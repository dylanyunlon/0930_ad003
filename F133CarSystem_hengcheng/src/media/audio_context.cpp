/*
 * audio_context.cpp
 *
 *  Created on: 2022年7月13日
 *      Author: ZKSWE Develop Team
 */

#include "audio_context.h"
#include "audio_linein.h"
#include "music_player.h"
#include "link/context.h"
#include "bt/context.h"
#include "uart/context.h"
#include "misc/storage.h"
#include "fy/files.hpp"

#define RECORD_VOL_KEY        "record_vol_key"
#define SYSTEM_VOL_KEY        "system_vol_key"
#define LYLINK_CALL_VOL_KEY   "lylink_call_vol_key"
#define AUDIO_OUTPUT_MODE_KEY "audio_output_mode"

namespace audio {

static float _s_record_vol;       // 录音音量
static float _s_system_vol;       // 系统音量  (mcu音量)

static float _s_lylink_call_vol;  // lylink通话音量 (相比于音乐声音很小，需要另外调节)

static Mutex _s_audio_mutex;
static audio_type_e _s_audio_type = E_AUDIO_TYPE_NULL;
static audio_source_e _s_audio_source = E_AUDIO_SOURCE_NULL;
static audio_player_mode_e _s_audio_output_mode = E_AUDIO_PLAYER_MODE_SPK;
static std::set<audio_output_mode_cb> _s_audio_output_mode_cb_set;

// 新增：通话前的音频类型记录
static audio_type_e _s_audio_type_before_call = E_AUDIO_TYPE_NULL;

typedef struct {
	audio_type_e type;			  // 音频类型
	bool playing;				  // 保存切换声音类型时的播放状态
	bool (*is_playing)();		  // 获取播放状态的方法
	void (*pause)();			  // 设置暂停状态的方法
	void (*resume)();			  // 设置恢复状态的方法
} audio_controller_t;

static void _linein_pause() {
	linein_stop();
}

static void _linein_resume() {
	linein_start();
}

static void _bt_music_pause() {
	bt::music_pause();
}

static void _bt_music_resume() {
	bt::music_play();
}

static audio_controller_t _s_audio_controller_tab[] = {
	{ E_AUDIO_TYPE_BT_MUSIC, false, bt::music_is_playing, _bt_music_pause, _bt_music_resume },
	{ E_AUDIO_TYPE_MUSIC, false, media::music_is_playing, media::music_pause, media::music_resume },
	{ E_AUDIO_TYPE_LYLINK_MUSIC, false, lk::music_is_playing, lk::music_pause, lk::music_resume },
	{ E_AUDIO_TYPE_LINEIN, false, linein_is_playing, _linein_pause, _linein_resume },
};

// 设置arm音量
static void _set_arm_vol(float vol) {
	zk_audio_player_set_volume(vol);
}

// 设置系统音量
static void _set_system_vol_imp(float vol) {
#if MEDIA_SOUND_USE_MCU
	uart::set_volume(vol);
#else
	_set_arm_vol(vol);      // arm vol
#endif
}

static inline bool _is_same_module(audio_type_e src_type, audio_type_e dst_type) {
	if (src_type == dst_type) {
		return true;
	}

	uint8_t src_module = src_type & E_AUDIO_MODULE_MASK;
	uint8_t dst_module = dst_type & E_AUDIO_MODULE_MASK;

	// 无线aa通话走蓝牙
	if ((dst_type == E_AUDIO_TYPE_BT_PHONE) &&
			((lk::get_lylink_type() == LINK_TYPE_WIFIAUTO) || (lk::get_lylink_type() == LINK_TYPE_USBAUTO)) ) {
		if (src_module == E_AUDIO_MODULE_LYLINK) {
			return true;
		}
	}

	return ((src_module == 0) || (dst_module == 0)) ? false : (src_module == dst_module);
}

// 选择音源
static void _select_audio_source(audio_type_e type) {
	audio_source_e source = E_AUDIO_SOURCE_NULL;

	switch (type) {
	case E_AUDIO_TYPE_BT_PHONE:
#if BT_MODULE == BT_MODULE_BLINK
		bt::set_bt_mute(false);
#endif
	case E_AUDIO_TYPE_BT_MUSIC:
		source = E_AUDIO_SOURCE_BT;
		break;

	case E_AUDIO_TYPE_RADIO:
		source = E_AUDIO_SOURCE_RADIO;
		break;

	case E_AUDIO_TYPE_MUSIC:
	case E_AUDIO_TYPE_VIDEO:
#if BT_MODULE == BT_MODULE_BLINK
		bt::set_bt_mute(true);
		break;
#endif
	default:
		source = E_AUDIO_SOURCE_MEDIA;
		break;
	}

	LOGD("type: %d  _s_audio_source  = %d   source = %d \n", type,_s_audio_source,source);
	if (_s_audio_source == source) {
		return;
	}

	LOGD("audio source: %d\n", source);
	_s_audio_source = source;

	switch (source) {
	case E_AUDIO_SOURCE_BT:
#if BT_MODULE == BT_MODULE_LYGOC
//		bt::sound_to_mcu();
		uart::set_sound_channel(SOUND_CHANNEL_BT);
#endif
		break;
	default:
#if BT_MODULE == BT_MODULE_LYGOC
//		bt::sound_to_arm();
		uart::set_sound_channel(SOUND_CHANNEL_ARM);
#endif
		break;
	}
}

void init() {
	// 设置alsa配置环境
	setenv("ALSA_CONFIG_DIR", "/res/ui/alsa", 1);
	// 设置插件路径
	setenv("ALSA_PLUGIN_DIR", "/res/lib", 0);
	// 禁用zkmedia调节音量
	setenv("ZKMEDIA_SOUND_UNADJABLE", "1", 0);
	// 限制最大音量
//	setenv("ZKMEDIA_SOUND_RANGE", "120", 0);
//	zk_audio_player_set_max_volume(MEDIA_SOUND_MAX_VOL);
	// 获取音量储存值
	_s_record_vol = storage::get_float(RECORD_VOL_KEY, 0.8);
	_s_system_vol = storage::get_float(SYSTEM_VOL_KEY, 0.7);
	_s_lylink_call_vol = storage::get_float(LYLINK_CALL_VOL_KEY, 0.8);

//	LOGD("--audio init-- _s_record_vol:%f  _s_system_vol:%f _s_media_vol:%f _s_lylink_call_vol:%f",
//			_s_record_vol, _s_system_vol, _s_media_vol, _s_lylink_call_vol);

#if MEDIA_SOUND_USE_MCU
	// arm音量固定最大值，只调节mcu音量
	_set_arm_vol(1.0);
#endif

	_set_system_vol_imp(_s_system_vol);
	zk_audio_record_set_volume(_s_record_vol);

	// 实例化空player
//	zk_audio_multi_player_init(2, 48000, 1024, 4);

	change_output_mode(get_output_mode());
}

//音源切换
void change_audio_type(audio_type_e type) {
	if (_s_audio_type == type) {
		return;
	}

	LOGD("change audio type: %d\n", type);

	// 新增：对于蓝牙音乐和视频之间的切换，需要特殊处理
	if ((_s_audio_type == E_AUDIO_TYPE_BT_MUSIC || type == E_AUDIO_TYPE_BT_MUSIC) &&
		(_s_audio_type == E_AUDIO_TYPE_VIDEO || type == E_AUDIO_TYPE_VIDEO)) {

		// 如果从蓝牙音乐切换到视频，确保蓝牙音乐保持播放状态（手机端继续播放）
		if (_s_audio_type == E_AUDIO_TYPE_BT_MUSIC && type == E_AUDIO_TYPE_VIDEO) {
			LOGD("Switching from BT music to video, BT music continues on phone\n");
			// 不暂停蓝牙音乐，让它在手机端继续播放
		}
		// 如果从视频切换回蓝牙音乐，检查蓝牙音乐是否还在播放
		else if (_s_audio_type == E_AUDIO_TYPE_VIDEO && type == E_AUDIO_TYPE_BT_MUSIC) {
			LOGD("Switching from video to BT music, checking BT music status\n");
			// 检查蓝牙音乐是否还在播放，如果没有则不切换音频源
			if (!bt::music_is_playing()) {
				LOGD("BT music is not playing, not switching audio source\n");
				return;
			}
		}
	}

	_s_audio_type = type;

	for (size_t i = 0; i < TAB_SIZE(_s_audio_controller_tab); ++i) {
		if (_is_same_module(_s_audio_controller_tab[i].type, type)) {
			continue;
		}

		// 修改：对于蓝牙音乐，如果正在播放视频，不暂停蓝牙音乐
		if (_s_audio_controller_tab[i].type == E_AUDIO_TYPE_BT_MUSIC && type == E_AUDIO_TYPE_VIDEO) {
			continue;
		}

		if (_s_audio_controller_tab[i].is_playing()) {
			_s_audio_controller_tab[i].pause();
		}
	}

	_select_audio_source(type);
}

audio_type_e get_audio_type() {
	return _s_audio_type;
}

//处理通话音频逻辑
void handle_phone(audio_type_e type, bool phoning) {
	LOGD("handle phone type: %d, phoning: %d\n", type, phoning);

	float vol = -1;

	switch (type) {
	case E_AUDIO_TYPE_BT_PHONE:
		// 通话声音较小，需要另外调节
		vol = phoning ? _s_lylink_call_vol : _s_system_vol;
		break;
	case E_AUDIO_TYPE_LYLINK_PHONE:
		// 通话声音较小，需要另外调节
		vol = phoning ? _s_lylink_call_vol : _s_system_vol;
		break;

	case E_AUDIO_TYPE_LYLINK_VR:    // 语音、来电铃声等
		break;

	default:
		break;
	}

	// 新增：记录通话前的音频类型
	if (phoning) {
		_s_audio_type_before_call = _s_audio_type;
		LOGD("Saving audio type before call: %d\n", _s_audio_type_before_call);
	}

	for (size_t i = 0; i < TAB_SIZE(_s_audio_controller_tab); ++i) {
		// 相同模块下不处理音频逻辑，由模块内部处理好
		if (_is_same_module(_s_audio_controller_tab[i].type, type)) {
			continue;
		}

		if (phoning) {
			// 记录通话前的状态
			if (_s_audio_controller_tab[i].is_playing()) {
				// 修改：对于蓝牙音乐，不暂停播放，让它在手机端继续
				if (_s_audio_controller_tab[i].type == E_AUDIO_TYPE_BT_MUSIC) {
					_s_audio_controller_tab[i].playing = true;
					LOGD("BT music continues playing on phone during call\n");
				} else {
					_s_audio_controller_tab[i].pause();
					_s_audio_controller_tab[i].playing = true;
				}
			}
		} else {
			// 恢复通话前的状态
			if (_s_audio_controller_tab[i].playing) {
				// 修改：通话结束后的恢复逻辑
				if (_s_audio_controller_tab[i].type == E_AUDIO_TYPE_BT_MUSIC) {
					// 检查蓝牙音乐是否还在播放且通话前是蓝牙音乐
					if (bt::music_is_playing() && _s_audio_type_before_call == E_AUDIO_TYPE_BT_MUSIC) {
						LOGD("Restoring BT music after call\n");
						_s_audio_controller_tab[i].resume();
						// 确保音频源切换到蓝牙音乐
						change_audio_type(E_AUDIO_TYPE_BT_MUSIC);
					}
				} else {
					// 其他音频源正常恢复
					_s_audio_controller_tab[i].resume();
				}
				_s_audio_controller_tab[i].playing = false;
			}
		}
	}

	DELAY(150);
	LOGD("--%d-- --%s-- vol:%.1f \n", __LINE__, __FILE__, vol);
	if (vol >= 0) {
		// arm静音
		LOGE("[audio] lk::get_lylink_type() = %d", lk::get_lylink_type());
		if (lk::get_lylink_type() == LINK_TYPE_WIFICP) {
			_set_arm_vol(0);
		} else {
			// 蓝牙通话需要直接修改mcu音量
			_set_system_vol_imp(0);
		}
	}

	_select_audio_source(phoning ? type : get_audio_type());
	LOGD("--%d-- --%s-- vol:%.1f \n", __LINE__, __FILE__, vol);
	if (vol >= 0) {
#if MEDIA_SOUND_USE_MCU
#if BT_MODULE == BT_MODULE_LYGOC
		if (type == E_AUDIO_TYPE_BT_PHONE && phoning) { bt::call_vol(_s_lylink_call_vol); }
#endif
		if (type == E_AUDIO_TYPE_BT_PHONE && !phoning){
			_set_system_vol_imp(vol);
		}else if(type == E_AUDIO_TYPE_LYLINK_PHONE){
			_set_system_vol_imp(vol);
		}
		DELAY(150);
		_set_arm_vol(1.0);
#else
		DELAY(150);
		_set_system_vol_imp(vol);
#endif
	}

	// 新增：通话结束后，如果需要恢复蓝牙音乐，确保音频路径正确
	if (!phoning && _s_audio_type_before_call == E_AUDIO_TYPE_BT_MUSIC && bt::music_is_playing()) {
		DELAY(200);  // 给系统一点时间完成音量设置
		bt::set_bt_mute(false, false);  // 确保蓝牙不是静音状态
		LOGD("Ensured BT audio path after call ended\n");
	}
}

void handle_tts(audio_type_e type, bool playing) {
	LOGD("handle tts type: %d, playing: %d\n", type, playing);

	switch (type) {
	case E_AUDIO_TYPE_LYLINK_TTS:
		// 导航播报压低媒体声音
		media::music_set_gain(playing ? 0.2 : 1.0);
		break;

	default:
		break;
	}

	_select_audio_source(playing ? type : get_audio_type());
}

void set_record_vol(float vol, bool effect) {
	_s_record_vol = vol;
	storage::put_float(RECORD_VOL_KEY, vol);
	if (effect) {
		zk_audio_record_set_volume(vol);
	}
}

void set_system_vol(float vol, bool effect) {
	_s_system_vol = vol;
	storage::put_float(SYSTEM_VOL_KEY, vol);
	if (effect) {
		_set_system_vol_imp(vol);
	}
}

void set_lylink_call_vol(float vol, bool effect) {
	_s_lylink_call_vol = vol;
	storage::put_float(LYLINK_CALL_VOL_KEY, vol);
	if (effect) {
		_set_system_vol_imp(vol);
	}
}

float get_record_vol() {
	return _s_record_vol;
}

float get_system_vol() {
	return _s_system_vol;
}

float get_lylink_call_vol() {
	return _s_lylink_call_vol;
}

void change_output_mode(audio_player_mode_e mode) {
	if (_s_audio_output_mode == mode) {
		return;
	}

	Mutex::Autolock _l(_s_audio_mutex);
	SET_FOREACH(_s_audio_output_mode_cb_set, audio_output_mode_cb, it) {
		(*it)(mode, _s_audio_output_mode);
	}

	zk_audio_player_set_mode(mode);
	storage::put_int(AUDIO_OUTPUT_MODE_KEY, mode);
	_s_audio_output_mode = mode;
}

audio_player_mode_e get_output_mode() {
	return (audio_player_mode_e) storage::get_int(AUDIO_OUTPUT_MODE_KEY, E_AUDIO_PLAYER_MODE_SPK);
}

void add_output_mode_cb(audio_output_mode_cb cb) {
	Mutex::Autolock _l(_s_audio_mutex);
	_s_audio_output_mode_cb_set.insert(cb);
}

void remove_output_mode_cb(audio_output_mode_cb cb) {
	Mutex::Autolock _l(_s_audio_mutex);
	_s_audio_output_mode_cb_set.erase(cb);
}

}
