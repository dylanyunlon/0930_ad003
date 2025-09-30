#pragma once
#include "uart/ProtocolSender.h"
/*
*此文件由GUI工具生成
*文件功能：用于处理用户的逻辑相应代码
*功能说明：
*========================onButtonClick_XXXX
当页面中的按键按下后系统会调用对应的函数，XXX代表GUI工具里面的[ID值]名称，
如Button1,当返回值为false的时候系统将不再处理这个按键，返回true的时候系统将会继续处理此按键。比如SYS_BACK.
*========================onSlideWindowItemClick_XXXX(int index)
当页面中存在滑动窗口并且用户点击了滑动窗口的图标后系统会调用此函数,XXX代表GUI工具里面的[ID值]名称，
如slideWindow1;index 代表按下图标的偏移值
*========================onSeekBarChange_XXXX(int progress)
当页面中存在滑动条并且用户改变了进度后系统会调用此函数,XXX代表GUI工具里面的[ID值]名称，
如SeekBar1;progress 代表当前的进度值
*========================ogetListItemCount_XXXX()
当页面中存在滑动列表的时候，更新的时候系统会调用此接口获取列表的总数目,XXX代表GUI工具里面的[ID值]名称，
如List1;返回值为当前列表的总条数
*========================oobtainListItemData_XXXX(ZKListView::ZKListItem *pListItem, int index)
 当页面中存在滑动列表的时候，更新的时候系统会调用此接口获取列表当前条目下的内容信息,XXX代表GUI工具里面的[ID值]名称，
如List1;pListItem 是贴图中的单条目对象，index是列表总目的偏移量。具体见函数说明
*========================常用接口===============
*LOGD(...)  打印调试信息的接口
*mTextXXXPtr->setText("****") 在控件TextXXX上显示文字****
*mButton1Ptr->setSelected(true); 将控件mButton1设置为选中模式，图片会切换成选中图片，按钮文字会切换为选中后的颜色
*mSeekBarPtr->setProgress(12) 在控件mSeekBar上将进度调整到12
*mListView1Ptr->refreshListView() 让mListView1 重新刷新，当列表数据变化后调用
*mDashbroadView1Ptr->setTargetAngle(120) 在控件mDashbroadView1上指针显示角度调整到120度
*
* 在Eclipse编辑器中  使用 "alt + /"  快捷键可以打开智能提示
*/

#include "net/context.h"
#include "link/context.h"
#include "uart/context.h"
#include "bt/context.h"
#include "media/audio_context.h"
#include "media/media_context.h"
#include "media/music_player.h"
#include "media/media_parser.h"
#include "system/setting.h"
#include "system/fm_emit.h"
#include "system/reverse.h"
#include "manager/LanguageManager.h"
#include "manager/ConfigManager.h"
#include "storage/StoragePreferences.h"
#include "misc/storage.h"
#include "fy/files.hpp"
#include "net/NetManager.h"
#include "os/MountMonitor.h"
#include "system/usb_monitor.h"
#include "tire/tire_parse.h"
#include "sysapp_context.h"
#include "utils/BitmapHelper.h"
#include <base/ui_handler.h>
#include "system/hardware.h"
#include "utils/TimeHelper.h"
#include "mode_observer.h"

#define WIFIMANAGER			NETMANAGER->getWifiManager()

#define QUERY_LINK_AUTH_TIMER	3
#define SWITCH_ADB_TIMER	4
#define BT_MUSIC_ID3		5
#define MUSIC_ERROR_TIMER	20

static bt_cb_t _s_bt_cb;
static bool _s_need_reopen_linkview;

// 添加页面状态管理
static int _current_page_index = 0;  // 当前页面索引
static bool _is_in_sub_pages = false; // 是否在子页面中

// **内存优化：添加状态管理变量**
static bool _is_in_reverse_mode = false;      // 倒车状态
static bool _is_ui_update_paused = false;     // UI更新暂停状态
static bool _is_music_info_cached = false;    // 音乐信息缓存状态
static std::string _cached_title = "";        // 缓存的音乐标题
static std::string _cached_artist = "";       // 缓存的艺术家信息
static std::string _last_play_file = "";      // 上次播放的文件
static bool _background_resources_loaded = false; // 背景资源加载状态
static bool _is_exiting_reverse = false;      // 新增：标记是否正在从倒车模式退出

static void init_default_language() {
    std::string current_code = LANGUAGEMANAGER->getCurrentCode();

    if (current_code.empty() || current_code == "zh_CN") {  // 当前语言代码为空、中文或系统首次启动的话就设置为英文
        if (!FILE_EXIST("/data/.language_init_flag")) {
            LOGD("First boot detected, setting default language to English");
            sys::setting::update_localescode("en_US");
            system("touch /data/.language_init_flag");
            LOGD("Default language set to English (en_US)");
        } else {  // 如果标志文件存在但语言仍是中文，不强制覆盖
            LOGD("Language init flag exists, current language: %s", current_code.c_str());
        }
    } else {
        LOGD("Current language already set: %s", current_code.c_str());
    }
}

static void _register_timer_fun(int id, int time) {
	mActivityPtr->registerUserTimer(id, time); // @suppress("无效参数")
}

static void _unregister_timer_fun(int id) {
	mActivityPtr->unregisterUserTimer(id); // @suppress("无效参数")
}

bool handle_main_page_back() {  // 页面返回处理函数
    if (mPageWindow1Ptr != NULL) {
        int currentPage = mPageWindow1Ptr->getCurrentPage();
        LOGD("Current page index: %d, _is_in_sub_pages: %s", currentPage, _is_in_sub_pages ? "true" : "false");

        if (_is_in_sub_pages || currentPage != 0) {
			mPageWindow1Ptr->setCurrentPage(0);  // 如果当前在子页面（music/video/photoalbum），返回到第一页
            _current_page_index = 0;
            _is_in_sub_pages = false;
            LOGD("Back to main page (page 0)");
            return true; // 返回true表示已处理返回事件
        } else {
            // 如果当前在第一页，返回false让系统处理（退出当前activity）
            LOGD("At main page, let system handle back");
            return false; // 让系统处理，会调用 EASYUICONTEXT->goBack()
        }
    }
    return false; // 返回false表示未处理，让系统继续处理
}

// 页面切换监听函数
static void onPageChange(int page) {
    _current_page_index = page;
    _is_in_sub_pages = (page != 0);
    LOGD("Page changed to: %d, is_in_sub_pages: %s", page, _is_in_sub_pages ? "true" : "false");
}

static void entry_lylink_ftu() {
	if (!sys::reverse_does_enter_status()) {
		EASYUICONTEXT->openActivity("lylinkviewActivity");
		_s_need_reopen_linkview = false;
	} else {
		LOGD("Is reverse status !!!\n");
		lk::video_stop();
		_s_need_reopen_linkview = true;
	}
}

static void _lylink_callback(LYLINKAPI_EVENT evt, int para0, void *para1) {
	switch (evt) {
	case LYLINK_LINK_ESTABLISH:
		LOGD("LYLINK_LINK_ESTABLISH %s", lk::_link_type_to_str((LYLINK_TYPE_E) para0));
		EASYUICONTEXT->hideStatusBar();		//隐藏蓝牙来电界面
		if (LINK_TYPE_AIRPLAY == para0 || LINK_TYPE_MIRACAST == para0 || LINK_TYPE_WIFILY == para0 || LINK_TYPE_WIFICP == para0) {
			if (bt::is_on()) {
				bt::power_off();
			}
			entry_lylink_ftu();
		}
		break;
	case LYLINK_LINK_DISCONN:
		LOGD("LYLINK_LINK_DISCONN........... %s", lk::_link_type_to_str((LYLINK_TYPE_E) para0));
		if (LINK_TYPE_AIRPLAY == para0 || LINK_TYPE_MIRACAST == para0 || LINK_TYPE_WIFILY == para0 || LINK_TYPE_WIFICP == para0) {
			if (!bt::is_on()) {
				bt::power_on();
			}
		}
		bt::query_state();
		EASYUICONTEXT->closeActivity("lylinkviewActivity");
		break;
	case LYLINK_PHONE_CONNECT:
		LOGD("LYLINK_PHONE_CONNECT %s", lk::_link_type_to_str((LYLINK_TYPE_E) para0));
		if (para0 == LINK_TYPE_WIFIAUTO || para0 == LINK_TYPE_WIFICP) {
			LOGD("You should open AP now.");
		}
		break;
	case LYLINK_FOREGROUND:
		LOGD("LYLINK_FOREGROUND");
		entry_lylink_ftu();
		break;
	case LYLINK_BACKGROUND:
	case LYLINK_HID_COMMAND:{
		if (evt == LYLINK_BACKGROUND) {
			LOGD("[main] LYLINK_BACKGROUND\n");
		} else {
			LOGD("[main] LYLINK_HID_COMMAND");
		}

		const char *app = EASYUICONTEXT->currentAppName();
		if (app && (strcmp(app, "lylinkviewActivity") == 0)) {
			EASYUICONTEXT->goHome();
		} else {
			EASYUICONTEXT->closeActivity("lylinkviewActivity");
		}
		_s_need_reopen_linkview = false;
	}
		break;
	case LYLINK_PHONE_DISCONN: 				// 蓝牙断开,不用处理
		LOGD("LYLINK_PHONE_DISCONN............. %s", lk::_link_type_to_str((LYLINK_TYPE_E) para0));
		lylinkapi_gocsdk("IA\r\n", strlen("IA\r\n"));
		break;
	default:
		break;
	}
}

// **内存优化：优化背景资源清理函数**
static void cleanup_background_resources() {
    if (_background_resources_loaded) {
        LOGD("[main] Cleaning up background resources");

        if (mTextViewBgPtr) {
            mTextViewBgPtr->setBackgroundBmp(NULL);
        }
        if (mPageWindow1Ptr) {
            mPageWindow1Ptr->setBackgroundBmp(NULL);
        }

        _background_resources_loaded = false;
    }
}

static void set_back_pic() {
	// **内存优化：避免重复加载背景资源**
	if (_background_resources_loaded) {
		return;
	}

	bitmap_t *bg_bmp = NULL;
	bitmap_t *bg1_bmp = NULL;
	BitmapHelper::loadBitmapFromFile(bg_bmp, CONFIGMANAGER->getResFilePath("/HomePage/carmain_home_wallpaper.jpg").c_str(), 3);
	BitmapHelper::loadBitmapFromFile(bg1_bmp, CONFIGMANAGER->getResFilePath("/HomePage/car_home_wallpaper_first.jpg").c_str(), 3);

	if (mTextViewBgPtr) {
		mTextViewBgPtr->setBackgroundBmp(bg_bmp);
	}
	if (mPageWindow1Ptr) {
		mPageWindow1Ptr->setBackgroundBmp(bg1_bmp);
	}

	_background_resources_loaded = true;
}


// **新增：倒车前窗口状态记录结构**
struct WindowState {
    bool window2_visible;
    bool window3_visible;
    bool window4_visible;
};

// **新增：添加窗口状态管理变量**
static WindowState _reverse_backup_window_state = {false, false, false};
static bool _has_backup_window_state = false;
static bool _is_transitioning_from_activity = false;  // 新增：标记是否从其他Activity过渡

void set_activity_transition_flag(bool flag) {
    _is_transitioning_from_activity = flag;
    LOGD("[main] Activity transition flag set to: %s", flag ? "true" : "false");
}

// **新增：记录当前窗口显示状态的函数**
static void backup_current_window_state() {
    if (mWindow2Ptr) {
        _reverse_backup_window_state.window2_visible = mWindow2Ptr->isVisible();
    }
    if (mWindow3Ptr) {
        _reverse_backup_window_state.window3_visible = mWindow3Ptr->isVisible();
    }
    if (mWindow4Ptr) {
        _reverse_backup_window_state.window4_visible = mWindow4Ptr->isVisible();
    }
    _has_backup_window_state = true;

    LOGD("[main] Backup window state - Window2: %s, Window3: %s, Window4: %s",
         _reverse_backup_window_state.window2_visible ? "visible" : "hidden",
         _reverse_backup_window_state.window3_visible ? "visible" : "hidden",
         _reverse_backup_window_state.window4_visible ? "visible" : "hidden");
}

// **新增：隐藏所有窗口的函数**
static void hide_all_windows() {
    LOGD("[main] Hiding all windows before reverse exit");
    if (mWindow2Ptr) {
        mWindow2Ptr->hideWnd();
    }
    if (mWindow3Ptr) {
        mWindow3Ptr->hideWnd();
    }
    if (mWindow4Ptr) {
        mWindow4Ptr->hideWnd();
    }
}

// **新增：恢复倒车前窗口状态的函数**
static void restore_backup_window_state() {
    if (!_has_backup_window_state) {
        LOGD("[main] No backup window state to restore");
        return;
    }

    LOGD("[main] Restoring backup window state - Window2: %s, Window3: %s, Window4: %s",
         _reverse_backup_window_state.window2_visible ? "show" : "keep hidden",
         _reverse_backup_window_state.window3_visible ? "show" : "keep hidden",
         _reverse_backup_window_state.window4_visible ? "show" : "keep hidden");

    if (_reverse_backup_window_state.window2_visible && mWindow2Ptr) {
        mWindow2Ptr->showWnd();
    }
    if (_reverse_backup_window_state.window3_visible && mWindow3Ptr) {
        mWindow3Ptr->showWnd();
    }
    if (_reverse_backup_window_state.window4_visible && mWindow4Ptr) {
        mWindow4Ptr->showWnd();
    }

    // 清除备份状态
    _has_backup_window_state = false;
}

// **修改：更新倒车状态回调函数**
static void _reverse_status_cb(int status) {
    LOGD("reverse status %d\n", status);
    base::runInUiThreadUniqueDelayed("rear_view_detection", [](){
        int status = sys::reverse_does_enter_status();
        LOGD("[main] reverse_status %d\n", status);
        if (status == REVERSE_STATUS_ENTER) {
            LOGD("[main] Entering reverse mode - pausing UI updates and cleaning resources");
            _is_in_reverse_mode = true;
            _is_ui_update_paused = true;
            _is_exiting_reverse = false;  // 进入倒车时清除退出标记

            // 清理背景资源以释放内存
            cleanup_background_resources();

            EASYUICONTEXT->openActivity("reverseActivity");
        } else {
            LOGD("[main] Exiting reverse mode - will manage windows and resume UI updates after delay");
            _is_in_reverse_mode = false;
            _is_exiting_reverse = true;   // 标记正在从倒车模式退出

            EASYUICONTEXT->closeActivity("reverseActivity");

            // **新增：倒车结束时立即记录窗口状态并隐藏所有窗口**
            backup_current_window_state();
            hide_all_windows();

            // 延迟恢复UI更新，避免内存峰值冲突
            base::runInUiThreadUniqueDelayed("resume_ui_updates", [](){
                _is_ui_update_paused = false;
                // 重新启动定时器
                _register_timer_fun(1, 20);

                // 倒车结束后延迟设置背景图片
                if (_is_exiting_reverse) {
                    set_back_pic();

                    // **新增：设置背景后恢复窗口显示状态**
                    restore_backup_window_state();

                    _is_exiting_reverse = false;  // 重置退出标记
                }

                LOGD("[main] UI updates resumed after reverse mode, background set, windows restored");
            }, 20); // 延迟1.5秒恢复

            if (_s_need_reopen_linkview) {
                _s_need_reopen_linkview = false;
                if (lk::is_connected()) {
                    EASYUICONTEXT->openActivity("lylinkviewActivity");
                }
            }
        }
    }, 20);
}

//// **内存优化：修改倒车状态回调函数**
//static void _reverse_status_cb(int status) {
//    LOGD("reverse status %d\n", status);
//    base::runInUiThreadUniqueDelayed("rear_view_detection", [](){
//        int status = sys::reverse_does_enter_status();
//        LOGD("[main] reverse_status %d\n", status);
//        if (status == REVERSE_STATUS_ENTER) {
//            LOGD("[main] Entering reverse mode - pausing UI updates and cleaning resources");
//            _is_in_reverse_mode = true;
//            _is_ui_update_paused = true;
//            _is_exiting_reverse = false;  // 进入倒车时清除退出标记
//
//            // 暂停音乐进度更新定时器
//            _unregister_timer_fun(1);
//
//            // 清理背景资源以释放内存
//            cleanup_background_resources();
//
//            EASYUICONTEXT->openActivity("reverseActivity");
//        } else {
//            LOGD("[main] Exiting reverse mode - will resume UI updates after delay");
//            _is_in_reverse_mode = false;
//            _is_exiting_reverse = true;   // 标记正在从倒车模式退出
//
//            EASYUICONTEXT->closeActivity("reverseActivity");
//
//            // 延迟恢复UI更新，避免内存峰值冲突
//            base::runInUiThreadUniqueDelayed("resume_ui_updates", [](){
//                _is_ui_update_paused = false;
//                // 重新启动定时器
//                _register_timer_fun(1, 20);
//
//                // 倒车结束后延迟设置背景图片
//                if (_is_exiting_reverse) {
//                    set_back_pic();
//                    _is_exiting_reverse = false;  // 重置退出标记
//                }
//
//                LOGD("[main] UI updates resumed after reverse mode, background set");
//            }, 20); // 延迟1.5秒恢复
//
//            if (_s_need_reopen_linkview) {
//                _s_need_reopen_linkview = false;
//                if (lk::is_connected()) {
//                    EASYUICONTEXT->openActivity("lylinkviewActivity");
//                }
//            }
//        }
//    }, 20);
//}


// **内存优化：优化音乐信息解析（增加缓存机制）**
static void parser() {
	std::string cur_play_file = media::music_get_current_play_file();

	// 如果文件没有改变且已缓存，直接使用缓存
	if (cur_play_file == _last_play_file && _is_music_info_cached && !_cached_title.empty()) {
		if (mtitleTextViewPtr) {
			mtitleTextViewPtr->setText(_cached_title);
		}
		if (martistTextViewPtr) {
			martistTextViewPtr->setText(_cached_artist);
			martistTextViewPtr->setLongMode(ZKTextView::E_LONG_MODE_SCROLL_CIRCULAR);
		}
		return;
	}

	_last_play_file = cur_play_file;

	id3_info_t info;
	memset(&info, 0, sizeof(id3_info_t));
	bool isTrue = media::parse_id3_info(cur_play_file.c_str(), &info);

	// 安全的字符串处理并缓存
	if (isTrue && info.title != nullptr && strlen(info.title) > 0) {
		_cached_title = std::string(info.title);
	} else {
		std::string file_name = fy::files::get_file_name(cur_play_file);
		if (isTrue && !file_name.empty()) {
			_cached_title = file_name;
		} else {
			_cached_title = "Unknown";
		}
	}

	if (isTrue && info.artist != nullptr && strlen(info.artist) > 0) {
		_cached_artist = std::string(info.artist);
	} else {
		_cached_artist = "Unknown";
	}

	// 更新UI
	if (mtitleTextViewPtr) {
		if (_cached_title == "Unknown") {
			mtitleTextViewPtr->setTextTr("Unknown");
		} else {
			mtitleTextViewPtr->setText(_cached_title);
		}
	}

	if (martistTextViewPtr) {
		if (_cached_artist == "Unknown") {
			martistTextViewPtr->setTextTr("Unknown");
		} else {
			martistTextViewPtr->setText(_cached_artist);
		}
		martistTextViewPtr->setLongMode(ZKTextView::E_LONG_MODE_SCROLL_CIRCULAR);
	}

	_is_music_info_cached = true;

}

static void _update_music_info() {
	bt_music_t music_info = bt::get_music_info();
	mtitleTextViewPtr->setText(music_info.title);
	martistTextViewPtr->setText(music_info.artist);
	martistTextViewPtr->setLongMode(ZKTextView::E_LONG_MODE_SCROLL_CIRCULAR);
}

static void _update_music_progress() {
	bt_music_t music_info = bt::get_music_info();

	mPlayProgressSeekbarPtr->setMax(music_info.duration);
	mPlayProgressSeekbarPtr->setProgress(music_info.curpos);
}

static void _bt_music_cb(bt_music_state_e state) {
	if (bt::music_is_playing()) {
		_update_music_info();
		_update_music_progress();
		mtitleTextViewPtr->setLongMode(ZKTextView::E_LONG_MODE_SCROLL_CIRCULAR);
		mtitleTextViewPtr->setTextColor(0xFFFFFFFF);
		mButtonPlayPtr->setSelected(true);
		sys::setting::set_music_play_dev(E_AUDIO_TYPE_BT_MUSIC);
	} else {
		mtitleTextViewPtr->setLongMode(ZKTextView::E_LONG_MODE_SCROLL_CIRCULAR);
		mtitleTextViewPtr->setTextColor(0xFFFFFFFF);
		mButtonPlayPtr->setSelected(false);
	}
}

static void _music_play_status_cb(music_play_status_e status) {
	switch (status) {
	case E_MUSIC_PLAY_STATUS_STARTED:     	// 播放开始
		parser();
		sys::setting::set_music_play_dev(E_AUDIO_TYPE_MUSIC);
		mButtonPlayPtr->setSelected(true);
		mtitleTextViewPtr->setLongMode(ZKTextView::E_LONG_MODE_SCROLL_CIRCULAR);
		mtitleTextViewPtr->setTextColor(0xFFFFFFFF);
		mPlayProgressSeekbarPtr->setMax(media::music_get_duration() / 1000);
		mPlayProgressSeekbarPtr->setProgress(0);
		break;
	case E_MUSIC_PLAY_STATUS_RESUME:     	// 恢复播放
		parser();
		sys::setting::set_music_play_dev(E_AUDIO_TYPE_MUSIC);
		mPlayProgressSeekbarPtr->setMax(media::music_get_duration() / 1000);
		mButtonPlayPtr->setSelected(true);
		mtitleTextViewPtr->setLongMode(ZKTextView::E_LONG_MODE_SCROLL_CIRCULAR);
		mtitleTextViewPtr->setTextColor(0xFFFFFFFF);
		break;
	case E_MUSIC_PLAY_STATUS_STOP:       	// 停止播放
		mPlayProgressSeekbarPtr->setMax(media::music_get_duration() / 1000);
		mButtonPlayPtr->setSelected(false);
		mtitleTextViewPtr->setLongMode(ZKTextView::E_LONG_MODE_SCROLL_CIRCULAR);
		mtitleTextViewPtr->setTextColor(0xFFFFFFFF);
		mtitleTextViewPtr->setTextTr("Unknown");
		martistTextViewPtr->setTextTr("Unknown");

		// 清理缓存
		_is_music_info_cached = false;
		_cached_title.clear();
		_cached_artist.clear();
		_last_play_file.clear();
		break;
	case E_MUSIC_PLAY_STATUS_PAUSE:      	// 暂停播放
		mtitleTextViewPtr->setLongMode(ZKTextView::E_LONG_MODE_NONE);
		mButtonPlayPtr->setSelected(false);
		mtitleTextViewPtr->setTextColor(0xFFFFFFFF);
		break;
	case E_MUSIC_PLAY_STATUS_COMPLETED:  	// 播放结束
		LOGE("[main] music play completed, will play next\n");
		media::music_next();
		break;
	case E_MUSIC_PLAY_STATUS_ERROR:      	// 播放错误
//		LOGE("music play error, will play next\n");
//		mActivityPtr->registerUserTimer(MUSIC_ERROR_TIMER, 3000);
		break;
	}
}

static void _bt_call_cb(bt_call_state_e state) {
	if (state != E_BT_CALL_STATE_IDLE) {
//		if (EASYUICONTEXT->isScreensaverEnable()) {
//			EASYUICONTEXT->screensaverOff();
//		}
		if (lk::get_lylink_type() == LINK_TYPE_WIFIAUTO) {
			const char *app = EASYUICONTEXT->currentAppName();
			if (!app) return;
			if(strcmp(app, "reverseActivity") == 0) {
				_s_need_reopen_linkview = true;
			} else if(strcmp(app, "lylinkviewActivity") != 0) {
				EASYUICONTEXT->openActivity("lylinkviewActivity");
			}
		}
	}
}

static void _bt_add_cb() {
	_s_bt_cb.call_cb = _bt_call_cb;
	_s_bt_cb.music_cb = _bt_music_cb;
	bt::add_cb(&_s_bt_cb);
}

static void _bt_remove_cb() {
	bt::remove_cb(&_s_bt_cb);
}

static bool _show_sys_info(unsigned long *freeram) {
	struct sysinfo info;
	int ret = 0;
	ret = sysinfo(&info);
	if(ret != 0) {
		return false;
	}
	*freeram = info.freeram;
	return true;
}

// **修改位置1: 添加时间显示功能函数 (新增)**
static void sys_time_(tm t) {
	const char* week[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
	char buf[128] = {0};
	char time[128] = {0};
	int hour = t.tm_hour;
    std::string ampmStr = LTOV((hour >= 12) ? "PM" : "AM");

	// 处理12小时制和24小时制
	if (!sys::setting::is_time_format_24h()) {
		hour = (hour > 12) ? (hour % 12) : hour;
		if (hour == 0) hour = 12;
		sprintf(time, "%02d:%02d %s", hour, t.tm_min, ampmStr.c_str());
	} else {
		sprintf(time, "%02d:%02d", hour, t.tm_min);
	}
	mtimeTextViewPtr->setText(time);

	sprintf(buf, "%04d/%02d/%02d", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);
	mdateTextViewPtr->setText(buf);
	mweekTextViewPtr->setText(LANGUAGEMANAGER->getValue(week[t.tm_wday%7]));
}

static void ctrl_UI_init() {
	EASYUICONTEXT->hideStatusBar();
}

namespace { // 加个匿名作用域，防止多个源文件定义相同类名，运行时冲突
 // 实现自己的监听接口
 class MyPageChangeListener : public ZKPageWindow::IPageChangeListener {
 public:
     virtual void onPageChange(ZKPageWindow *pPageWindow, int page) {
         LOGD("当前切换到第%d页", page);
         ::onPageChange(page);  // 调用我们的页面切换处理函数
     }
 };

 static MyPageChangeListener _s_page_change_listener;
 }

namespace{
class AppSlidePageChangeListener : public ZKSlideWindow::ISlidePageChangeListener {
protected:
	virtual void onSlidePageChange(ZKSlideWindow *pSlideWindow, int page) {
		mStatusRadioGroupPtr->setCheckedID((page == 0) ? ID_MAIN_RadioButton0 : ID_MAIN_RadioButton1);
	}
};
}
static AppSlidePageChangeListener _s_app_slide_page_change_listener;


static void _preload_resources() {
	const char *pic_tab[] = {
		"/res/font/sanswithfz_jp_it_pt.ttf",
		"navi/bg_bt_n.png",
		"navi/bg_bt_p.png",
		"navi/bg_eq_n.png",
		"navi/bg_eq_p.png",
		"navi/bg_fm_n.png",
		"navi/bg_fm_p.png",
		"navi/bg_screen_off_n.png",
		"navi/bg_screen_off_p.png",
		"navi/icon_btvoice.png",
		"navi/icon_light.png",
		"navi/icon_setting_n.png",
		"navi/icon_setting_p.png",
		"navi/icon_voice.png",
		"navi/progress_n.png",
		"navi/progress_p.png",
	};

	LOGD("[main] preload resources start\n");

	size_t size = TAB_SIZE(pic_tab);
	for (size_t i = 0; i < size; ++i) {
		if (i == 0) {
			fy::cache_file(pic_tab[i]);
		} else {
			fy::cache_file(CONFIGMANAGER->getResFilePath(pic_tab[i]));
		}
	}

	LOGD("[main] preload resources end\n");

	return ;
}
/**
 * 注册定时器
 * 填充数组用于注册定时器
 * 注意：id不能重复
 */
static S_ACTIVITY_TIMEER REGISTER_ACTIVITY_TIMER_TAB[] = {
//	{0,  1000}, //定时器id=0, 时间间隔1秒
	{1,  1000},
	{QUERY_LINK_AUTH_TIMER, 6000},
	{SWITCH_ADB_TIMER, 1000}, // 延迟打开ADB
};

/**
 * 当界面构造时触发
 */
static void onUI_init() {
    //Tips :添加 UI初始化的显示代码到这里,如:mText1Ptr->setText("123");
    // f133-B 文件系统加载慢，导致上电第一次滑动ui很慢，提前cach文件
//	system("cp /res/font/sans.ttf /dev/null");
    _preload_resources();
    ctrl_UI_init();

    // **内存优化：初始化状态变量**
    _is_in_reverse_mode = false;
    _is_ui_update_paused = false;
    _is_music_info_cached = false;
    _background_resources_loaded = false;
    _is_exiting_reverse = false;  // 初始化新增的状态变量
    _cached_title.clear();
    _cached_artist.clear();
    _last_play_file.clear();

    // 系统设置初始化
    sys::setting::init();
    // 打开温控线程
    sys::hw::init();

    // 串口初始化
    uart::init();
    // 上电mute
    uart::set_amplifier_mute(0);

    // 蓝牙初始化
    bt::init();
	mActivityPtr->registerUserTimer(BT_MUSIC_ID3, 0);

    // 网络初始化
    net::init();

    // 媒体初始化
    media::init();
    media::music_add_play_status_cb(_music_play_status_cb);

    // 启动手机互联
    lk::add_lylink_callback(_lylink_callback);
    lk::start_lylink();

    app::attach_timer(_register_timer_fun, _unregister_timer_fun);

    // 启动倒车检测
    sys::reverse_add_status_cb(_reverse_status_cb);
    sys::reverse_detect_start();

    _bt_add_cb();
    bt::query_state();

    media::music_add_play_status_cb(_music_play_status_cb);
    mTextView1Ptr->setTouchPass(true);
    martistTextViewPtr->setTouchPass(true);
//	mappSlideWindowPtr->setSlidePageChangeListener(&_s_app_slide_page_change_listener);
    mStatusRadioGroupPtr->setCheckedID(ID_MAIN_RadioButton0);
    mpage3RadioGroupPtr->setCheckedID(ID_MAIN_RadioButton3);
    if(bt::is_calling()){
        bt::call_vol(audio::get_lylink_call_vol());
    }

    base::UiHandler::implementTimerRegistration([]() {
        mActivityPtr->registerUserTimer(base::TIMER_UI_HANDLER, 0); // @suppress("无效参数")
    });

    // 注册页面切换监听器
    if (mPageWindow1Ptr != NULL) {
        mPageWindow1Ptr->setPageChangeListener(&_s_page_change_listener);
    }


}

/**
 * 当切换到该界面时触发
 */
static void onUI_intent(const Intent *intentPtr) {
    if (intentPtr != NULL) {
        //TODO
    }
}

/*
 * 当界面显示时触发
 */
static void onUI_show() {
    mode::set_switch_mode(E_SWITCH_MODE_NULL);

    // **内存优化：重置状态**
    _is_ui_update_paused = false;
    _is_in_reverse_mode = false;

    int curPos = -1;

    // **新增：检查是否从其他Activity过渡回来**
    if (_is_transitioning_from_activity) {
        LOGD("[main] Transitioning from activity - delaying background setup");

        // 先隐藏窗口
        backup_current_window_state();
        hide_all_windows();

        // 延迟设置背景和恢复窗口
        base::runInUiThreadUniqueDelayed("activity_transition_setup", [](){
            set_back_pic();
            restore_backup_window_state();
            _is_transitioning_from_activity = false;
            LOGD("[main] Activity transition complete, background and windows restored");
        }, 20);
    } else if (!_is_in_reverse_mode && !_is_exiting_reverse) {
        // 正常情况下直接设置背景图片
        set_back_pic();
    }

    if (sys::setting::get_music_play_dev() == E_AUDIO_TYPE_BT_MUSIC) {
        _update_music_info();
        _update_music_progress();
        if (bt::music_is_playing()) {
            mButtonPlayPtr->setSelected(true);
        }
    } else if (sys::setting::get_music_play_dev() == E_AUDIO_TYPE_MUSIC) {
        parser();
        if (media::music_is_playing()) {
            mButtonPlayPtr->setSelected(true);
            curPos = media::music_get_current_position() / 1000;
            mPlayProgressSeekbarPtr->setMax(media::music_get_duration() / 1000);
            mtitleTextViewPtr->setLongMode(ZKTextView::E_LONG_MODE_SCROLL_CIRCULAR);
            mtitleTextViewPtr->setTextColor(0xFFFFFFFF);
        }
    } else {
    }
    if (curPos >= 0) {
        mPlayProgressSeekbarPtr->setProgress(curPos);
    }
    if (!app::is_show_topbar()) {
        sys::setting::set_reverse_topbar_show(true);
        app::show_topbar();
    }
    init_default_language();
}

/*
 * 当界面隐藏时触发
 */
static void onUI_hide() {
	LOGD("[main] onUI_hide - cleaning up resources");

	// **内存优化：暂停UI更新**
	_is_ui_update_paused = true;

	// **内存优化：清理背景资源**
	cleanup_background_resources();

	// **内存优化：清理音乐信息缓存**
	_is_music_info_cached = false;
	_cached_title.clear();
	_cached_artist.clear();
}

/*
 * 当界面完全退出时触发
 */
static void onUI_quit() {
	lk::remove_lylink_callback(_lylink_callback);
	media::music_remove_play_status_cb(_music_play_status_cb);
	mPlayProgressSeekbarPtr->setSeekBarChangeListener(NULL);
//	mappSlideWindowPtr->setSlidePageChangeListener(NULL);

	// **内存优化：完全清理资源**
	cleanup_background_resources();
	_is_music_info_cached = false;
	_cached_title.clear();
	_cached_artist.clear();
	_last_play_file.clear();

	_bt_remove_cb();

	// 注销页面切换监听器
	if (mPageWindow1Ptr != NULL) {
		mPageWindow1Ptr->setPageChangeListener(NULL);
	}
}

/**
 * 串口数据回调接口
 */
static void onProtocolDataUpdate(const SProtocolData &data) {

}

/**
 * 定时器触发函数
 * 不建议在此函数中写耗时操作，否则将影响UI刷新
 * 参数： id
 *         当前所触发定时器的id，与注册时的id相同
 * 返回值: true
 *             继续运行当前定时器
 *         false
 *             停止运行当前定时器
 */
static bool onUI_Timer(int id) {
	if (app::on_timer(id)) {
		return false;
	}
	switch (id) {
	case 0: {
		unsigned long freeram = 0;
		bool ret = _show_sys_info(&freeram);
		if(ret) {
			LOGD("-----------Current MemFree: %ldKB, reverse: %s, ui_paused: %s---------------",
				freeram >> 10,
				_is_in_reverse_mode ? "true" : "false",
				_is_ui_update_paused ? "true" : "false");
		} else {
			LOGD("-----------get MemFree info fail----------------");
		}
	}
		break;
	case 1: {
        // **内存优化：在倒车模式或UI暂停时跳过更新**
        if (_is_ui_update_paused || _is_in_reverse_mode) {
            return true;
        }

        int curPos = -1;
        if (sys::setting::get_music_play_dev() == E_AUDIO_TYPE_MUSIC) {
            if (media::music_is_playing()) {
                curPos = media::music_get_current_position() / 1000;
            }
            if (curPos >= 0) {
            	mPlayProgressSeekbarPtr->setProgress(curPos);
            }
        }
        // **内存优化：减少时间更新频率**
        static int time_update_counter = 0;
        if (++time_update_counter >= 3) { // 每3秒更新一次时间显示
            time_update_counter = 0;
            sys_time_(*TimeHelper::getDateTime());
        }
	}
		break;
	case QUERY_LINK_AUTH_TIMER:		// 查询互联授权状态
		lk::query_is_authorized();
		return false;
	case SWITCH_ADB_TIMER: {
		if (strcmp("UpgradeActivity", EASYUICONTEXT->currentAppName()) == 0) {
			app::hide_topbar();
		}
		if (sys::setting::is_usb_adb_enabled()) {
			if (sys::get_usb_mode() != E_USB_MODE_DEVICE) {
				sys::change_usb_mode(E_USB_MODE_DEVICE);
			}
		} else {
			sys::set_usb_config(E_USB_MODE_HOST);
		}

		// unmute
		uart::set_amplifier_mute(1);
	}
		return false;
	case BT_MUSIC_ID3:
		if (bt::music_is_playing()) {
			bt::query_music_info();
		}
		return false;
	case MUSIC_ERROR_TIMER:
		media::music_next(true);
		return false;
    case base::TIMER_UI_HANDLER:
      return base::UiHandler::onTimer();
      break;
	default:
		break;
	}
    return true;
}

/**
 * 有新的触摸事件时触发
 * 参数：ev
 *         新的触摸事件
 * 返回值：true
 *            表示该触摸事件在此被拦截，系统不再将此触摸事件传递到控件上
 *         false
 *            触摸事件将继续传递到控件上
 */
static bool onmainActivityTouchEvent(const MotionEvent &ev) {
	LayoutPosition pos = EASYUICONTEXT->getNaviBar()->getPosition();
	if (pos.mTop != -pos.mHeight) {	return false; } 	// 导航栏下滑了
	switch (ev.mActionStatus) {
	case MotionEvent::E_ACTION_DOWN:	//触摸按下
		break;
	case MotionEvent::E_ACTION_MOVE:	//触摸滑动
		break;
	case MotionEvent::E_ACTION_UP:  	//触摸抬起
		break;
	default:
		break;
	}
	return false;
}
static bool onButtonClick_NextButton(ZKButton *pButton) {
    LOGD(" ButtonClick NextButton !!!\n");
	if (lk::is_connected()) {
		mlinkTipsWindowPtr->showWnd();
		return false;
	}
	if (sys::setting::get_music_play_dev() == E_AUDIO_TYPE_MUSIC) {
	    media::music_next(true);
	} else if (sys::setting::get_music_play_dev() == E_AUDIO_TYPE_BT_MUSIC) {
		bt::music_next();
	}
    return false;
}

static bool onButtonClick_ButtonPlay(ZKButton *pButton) {
    LOGD(" ButtonClick ButtonPlay !!!\n");

	if (lk::is_connected()) {
		mlinkTipsWindowPtr->showWnd();
		return false;
	}

	if (sys::setting::get_music_play_dev() == E_AUDIO_TYPE_MUSIC) {
	    if (media::music_get_play_index() == -1) {
	    	return false;
	    } else if (media::music_is_playing()) {
	        media::music_pause();
	    } else {
	    	media::music_resume();
	    }
	} else if (sys::setting::get_music_play_dev() == E_AUDIO_TYPE_BT_MUSIC) {
	    bt::music_is_playing() ? bt::music_pause() : bt::music_play();
	}
    return false;
}

static bool onButtonClick_PrevButton(ZKButton *pButton) {
    LOGD(" ButtonClick PrevButton !!!\n");
	if (lk::is_connected()) {
		mlinkTipsWindowPtr->showWnd();
		return false;
	}
	if (sys::setting::get_music_play_dev() == E_AUDIO_TYPE_MUSIC) {
	    media::music_prev(true);
	} else if (sys::setting::get_music_play_dev() == E_AUDIO_TYPE_BT_MUSIC) {
		bt::music_prev();
	}
    return false;
}

static void onProgressChanged_PlayProgressSeekbar(ZKSeekBar *pSeekBar, int progress) {
    //LOGD(" ProgressChanged PlayProgressSeekbar %d !!!\n", progress);
}

static bool onButtonClick_Setting(ZKButton *pButton) {
    LOGD(" ButtonClick Setting !!!\n");
    EASYUICONTEXT->openActivity("settingsActivity");
    return false;
}

static bool onButtonClick_ToMusic(ZKButton *pButton) {
    LOGD(" ButtonClick ToMusic !!!\n");
	if (lk::is_connected()) {
		mlinkTipsWindowPtr->showWnd();
		return false;
	}

	if (sys::setting::get_music_play_dev() == E_AUDIO_TYPE_MUSIC) {
	    EASYUICONTEXT->openActivity("musicActivity");
	} else if (sys::setting::get_music_play_dev() == E_AUDIO_TYPE_BT_MUSIC) {
	    EASYUICONTEXT->openActivity("btMusicActivity");
	}
    return false;
}

static void open_linkhelp_activity(link_mode_e mode) {
	Intent *i = new Intent;
	i->putExtra("link_mode", fy::format("%d", mode));
	LOGD("[main] choose link mode %s\n", sys::setting::get_link_mode_str(mode));
	EASYUICONTEXT->openActivity("linkhelpActivity", i);
}

static void _change_link_app(link_mode_e mode) {
	switch (mode) {
	case E_LINK_MODE_HICAR:
	case E_LINK_MODE_ANDROIDAUTO:
	case E_LINK_MODE_CARPLAY:
		if (net::get_mode() != E_NET_MODE_AP) { net::change_mode(E_NET_MODE_AP);}
		break;
	case E_LINK_MODE_AIRPLAY:
		if (net::get_mode() != E_NET_MODE_AP) { net::change_mode(E_NET_MODE_AP); }
		break;
	case E_LINK_MODE_CARLIFE:
		if (net::get_mode() != E_NET_MODE_WIFI) { net::change_mode(E_NET_MODE_WIFI); }
		break;
	case E_LINK_MODE_MIRACAST:
	case E_LINK_MODE_LYLINK:
		if (net::get_mode() != E_NET_MODE_P2P) { net::change_mode(E_NET_MODE_P2P); }
		break;
	default:
		break;
	}
	open_linkhelp_activity(mode);
}

static void open_link_activity(link_mode_e mode) {
	LYLINK_TYPE_E link_type = lk::get_lylink_type();
	switch(mode) {
	case E_LINK_MODE_CARPLAY:
		if ((link_type == LINK_TYPE_WIFICP) || (link_type == LINK_TYPE_USBCP)) {
			EASYUICONTEXT->openActivity("lylinkviewActivity");
			return;
		}
		break;
	case E_LINK_MODE_ANDROIDAUTO:
		if ((link_type == LINK_TYPE_WIFIAUTO) || (link_type == LINK_TYPE_USBAUTO)) {
			EASYUICONTEXT->openActivity("lylinkviewActivity");
			return;
		}
		break;
	case E_LINK_MODE_CARLIFE:
		if ((link_type == LINK_TYPE_WIFILIFE) || (link_type == LINK_TYPE_USBLIFE)) {
			EASYUICONTEXT->openActivity("lylinkviewActivity");
			return;
		}
		break;
	case E_LINK_MODE_HICAR:
		if ((link_type == LINK_TYPE_WIFIHICAR) || (link_type == LINK_TYPE_USBHICAR)) {
			EASYUICONTEXT->openActivity("lylinkviewActivity");
			return;
		}
		break;
	case E_LINK_MODE_MIRACAST:
		if (link_type == LINK_TYPE_MIRACAST) {
			EASYUICONTEXT->openActivity("lylinkviewActivity");
			return;
		}
		break;
	case E_LINK_MODE_LYLINK:
		if (link_type == LINK_TYPE_WIFILY) {
			EASYUICONTEXT->openActivity("lylinkviewActivity");
			return;
		}
		break;
	case E_LINK_MODE_AIRPLAY:
		if (link_type == LINK_TYPE_AIRPLAY) {
			EASYUICONTEXT->openActivity("lylinkviewActivity");
			return;
		}
		break;
	default:
		break;
	}
	if (lk::is_connected()) {
		mlinkTipsWindowPtr->showWnd();
		return;
	}

	if (mode == E_LINK_MODE_AIRPLAY || mode == E_LINK_MODE_LYLINK || mode ==E_LINK_MODE_MIRACAST) {
		if (bt::is_on()) {
			bt::power_off();
		}
	} else {
		if (!bt::is_on()) {
			bt::power_on();
		}
	}

	open_linkhelp_activity(mode);
//	_change_link_app(mode);
}

static bool onButtonClick_Button1(ZKButton *pButton) {
    LOGD(" ButtonClick Button1 !!!\n");
    EASYUICONTEXT->openActivity("mcubtUpdActivity");
//    EASYUICONTEXT->openActivity("TestRecordActivity");
    return false;
}

static bool onButtonClick_Button2(ZKButton *pButton) {
    LOGD(" ButtonClick Button2 !!!\n");
   // bt::query_mf();
    EASYUICONTEXT->openActivity("soundEffectActivity");
    return false;
}

static void onCheckedChanged_StatusRadioGroup(ZKRadioGroup* pRadioGroup, int checkedID) {
    LOGD(" RadioGroup StatusRadioGroup checked %d", checkedID);
}

static void onProgressChanged_PlayVolSeekBar(ZKSeekBar *pSeekBar, int progress) {
    //LOGD(" ProgressChanged PlayVolSeekBar %d !!!\n", progress);
}
static bool onButtonClick_button_apps(ZKButton *pButton) {
    LOGD(" ButtonClick button_apps !!!\n");
    return false;
}
static bool onButtonClick_appsButton(ZKButton *pButton) {
    LOGD(" ButtonClick appsButton !!!\n");
    mPageWindow1Ptr->turnToNextPage(false);
    return false;
}

static bool onButtonClick_settingButton(ZKButton *pButton) {
    LOGD(" ButtonClick settingButton !!!\n");
    EASYUICONTEXT->openActivity("setshowActivity");
    return false;
}

static bool onButtonClick_videoButton(ZKButton *pButton) {
    LOGD(" ButtonClick videoButton !!!\n");
    if (lk::is_connected()) {
        mlinkTipsWindowPtr->showWnd();
        return false;
    }
    EASYUICONTEXT->openActivity("videoActivity");
    return false;
}

static bool onButtonClick_musicButton(ZKButton *pButton) {
    LOGD(" ButtonClick musicButton !!!\n");
    if (lk::is_connected()) {
        mlinkTipsWindowPtr->showWnd();
        return false;
    }
    EASYUICONTEXT->openActivity("musicActivity");
    return false;
}

static bool onButtonClick_btButton(ZKButton *pButton) {
    LOGD(" ButtonClick btButton !!!\n");
    if (lk::is_connected()) {
        mlinkTipsWindowPtr->showWnd();
        return false;
    }
    EASYUICONTEXT->openActivity("btsettingActivity");
    return false;
}

static bool onButtonClick_volumeoutputButton(ZKButton *pButton) {
    LOGD(" ButtonClick volumeoutputButton !!!\n");
    // 这个按钮在原函数中没有对应的Activity，可能需要添加具体逻辑
    EASYUICONTEXT->openActivity("FMemitActivity");
    return false;
}

static bool onButtonClick_airpalyButton(ZKButton *pButton) {
    LOGD(" ButtonClick airpalyButton !!!\n");
    open_link_activity(E_LINK_MODE_AIRPLAY);
    return false;
}

static bool onButtonClick_carplayButton(ZKButton *pButton) {
    LOGD(" ButtonClick carplayButton !!!\n");
    open_link_activity(E_LINK_MODE_CARPLAY);
    return false;
}

static bool onButtonClick_androidautoButton(ZKButton *pButton) {
    LOGD(" ButtonClick androidautoButton !!!\n");
    open_link_activity(E_LINK_MODE_ANDROIDAUTO);
    return false;
}
static void onCheckedChanged_RadioGroup1(ZKRadioGroup* pRadioGroup, int checkedID) {
    LOGD(" RadioGroup RadioGroup1 checked %d", checkedID);
}

static bool onButtonClick_audiooutputButton(ZKButton *pButton) {
    LOGD(" ButtonClick audiooutputButton !!!\n");
    EASYUICONTEXT->openActivity("FMemitActivity");
    return false;
}
static void onCheckedChanged_page3RadioGroup(ZKRadioGroup* pRadioGroup, int checkedID) {
    LOGD(" RadioGroup page3RadioGroup checked %d", checkedID);
}
static bool onButtonClick_carplayPage2Button(ZKButton *pButton) {
    LOGD(" ButtonClick carplayPage2Button !!!\n");
    LOGD(" ButtonClick CPButton !!!\n");
	open_link_activity(E_LINK_MODE_CARPLAY);
    return false;
}

static bool onButtonClick_androidautoPage2Button(ZKButton *pButton) {
    LOGD(" ButtonClick androidautoPage2Button !!!\n");
    open_link_activity(E_LINK_MODE_ANDROIDAUTO);
    return false;
}

static bool onButtonClick_airplayPage2Button(ZKButton *pButton) {
    LOGD(" ButtonClick airplayPage2Button !!!\n");
    open_link_activity(E_LINK_MODE_AIRPLAY);
    return false;
}

static bool onButtonClick_aicastPage2Button(ZKButton *pButton) {
    LOGD(" ButtonClick aicastPage2Button !!!\n");
    open_link_activity(E_LINK_MODE_LYLINK);
    return false;
}

static bool onButtonClick_miracastPage2Button(ZKButton *pButton) {
    LOGD(" ButtonClick miracastPage2Button !!!\n");
    open_link_activity(E_LINK_MODE_MIRACAST);
    return false;
}

static bool onButtonClick_bluetoothPage2Button(ZKButton *pButton) {
    LOGD(" ButtonClick bluetoothPage2Button !!!\n");
    if (lk::is_connected()) {
		mlinkTipsWindowPtr->showWnd();
		return false;
	}
	EASYUICONTEXT->openActivity("btsettingActivity");
    return false;
}

static bool onButtonClick_musicPage2Button(ZKButton *pButton) {
    LOGD(" ButtonClick musicPage2Button !!!\n");
    if (lk::is_connected()) {
		mlinkTipsWindowPtr->showWnd();
		return false;
	}
	EASYUICONTEXT->openActivity("musicActivity");
    return false;
}

static bool onButtonClick_videoPage2Button(ZKButton *pButton) {
    LOGD(" ButtonClick videoPage2Button !!!\n");
    if (lk::is_connected()) {
		mlinkTipsWindowPtr->showWnd();
		return false;
	}
	EASYUICONTEXT->openActivity("videoActivity");
    return false;
}

static bool onButtonClick_albumPage2Button(ZKButton *pButton) {
    LOGD(" ButtonClick albumPage2Button !!!\n");
    if (lk::is_connected()) {
			mlinkTipsWindowPtr->showWnd();
			return false;
		}
	EASYUICONTEXT->openActivity("PhotoAlbumActivity");
    return false;
}

static bool onButtonClick_settingPage2Button(ZKButton *pButton) {
    LOGD(" ButtonClick settingPage2Button !!!\n");
    EASYUICONTEXT->openActivity("setshowActivity");
    return false;
}
