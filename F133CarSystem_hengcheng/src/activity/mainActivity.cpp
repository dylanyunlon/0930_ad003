/***********************************************
/gen auto by zuitools
***********************************************/
#include "mainActivity.h"

/*TAG:GlobalVariable全局变量*/
static ZKTextView* mmusicPicTextViewPtr;
static ZKButton* msettingPage2ButtonPtr;
static ZKButton* malbumPage2ButtonPtr;
static ZKButton* mvideoPage2ButtonPtr;
static ZKButton* mmusicPage2ButtonPtr;
static ZKButton* mbluetoothPage2ButtonPtr;
static ZKButton* mmiracastPage2ButtonPtr;
static ZKButton* maicastPage2ButtonPtr;
static ZKButton* mairplayPage2ButtonPtr;
static ZKButton* mandroidautoPage2ButtonPtr;
static ZKButton* mcarplayPage2ButtonPtr;
static ZKRadioGroup* mpage3RadioGroupPtr;
static ZKButton* maudiooutputButtonPtr;
static ZKWindow* mWindow4Ptr;
static ZKTextView* mtimeTextViewPtr;
static ZKTextView* mweekTextViewPtr;
static ZKTextView* mdateTextViewPtr;
static ZKButton* mandroidautoButtonPtr;
static ZKButton* mcarplayButtonPtr;
static ZKButton* mairpalyButtonPtr;
static ZKButton* mvolumeoutputButtonPtr;
static ZKButton* mbtButtonPtr;
static ZKButton* mmusicButtonPtr;
static ZKButton* mvideoButtonPtr;
static ZKButton* msettingButtonPtr;
static ZKButton* mappsButtonPtr;
static ZKTextView* mTextView4Ptr;
static ZKRadioGroup* mStatusRadioGroupPtr;
static ZKWindow* mWindow3Ptr;
static ZKWindow* mWindow2Ptr;
static ZKPageWindow* mPageWindow1Ptr;
static ZKTextView* mTextView6Ptr;
static ZKWindow* mWindow5Ptr;
static ZKWindow* mlinkTipsWindowPtr;
static ZKTextView* mTextView3Ptr;
static ZKWindow* mWindow1Ptr;
static ZKWindow* mbtTipsWindowPtr;
static ZKTextView* martistTextViewPtr;
static ZKTextView* mTextView1Ptr;
static ZKButton* mToMusicPtr;
static ZKSeekBar* mPlayProgressSeekbarPtr;
static ZKTextView* mTextView2Ptr;
static ZKButton* mPrevButtonPtr;
static ZKButton* mButtonPlayPtr;
static ZKButton* mNextButtonPtr;
static ZKTextView* mtitleTextViewPtr;
static ZKWindow* mmusicWindowPtr;
static ZKTextView* mTextViewBgPtr;
static mainActivity* mActivityPtr;

/*register activity*/
REGISTER_ACTIVITY(mainActivity);

typedef struct {
	int id; // 定时器ID ， 不能重复
	int time; // 定时器  时间间隔  单位 毫秒
}S_ACTIVITY_TIMEER;

#include "logic/mainLogic.cc"

/***********/
typedef struct {
    int id;
    const char *pApp;
} SAppInfo;

/**
 *点击跳转window
 */
static SAppInfo sAppInfoTab[] = {
//  { ID_MAIN_TEXT, "TextViewActivity" },
};

/***************/
typedef bool (*ButtonCallback)(ZKButton *pButton);
/**
 * button onclick表
 */
typedef struct {
    int id;
    ButtonCallback callback;
}S_ButtonCallback;

/*TAG:ButtonCallbackTab按键映射表*/
static S_ButtonCallback sButtonCallbackTab[] = {
    ID_MAIN_settingPage2Button, onButtonClick_settingPage2Button,
    ID_MAIN_albumPage2Button, onButtonClick_albumPage2Button,
    ID_MAIN_videoPage2Button, onButtonClick_videoPage2Button,
    ID_MAIN_musicPage2Button, onButtonClick_musicPage2Button,
    ID_MAIN_bluetoothPage2Button, onButtonClick_bluetoothPage2Button,
    ID_MAIN_miracastPage2Button, onButtonClick_miracastPage2Button,
    ID_MAIN_aicastPage2Button, onButtonClick_aicastPage2Button,
    ID_MAIN_airplayPage2Button, onButtonClick_airplayPage2Button,
    ID_MAIN_androidautoPage2Button, onButtonClick_androidautoPage2Button,
    ID_MAIN_carplayPage2Button, onButtonClick_carplayPage2Button,
    ID_MAIN_audiooutputButton, onButtonClick_audiooutputButton,
    ID_MAIN_androidautoButton, onButtonClick_androidautoButton,
    ID_MAIN_carplayButton, onButtonClick_carplayButton,
    ID_MAIN_airpalyButton, onButtonClick_airpalyButton,
    ID_MAIN_volumeoutputButton, onButtonClick_volumeoutputButton,
    ID_MAIN_btButton, onButtonClick_btButton,
    ID_MAIN_musicButton, onButtonClick_musicButton,
    ID_MAIN_videoButton, onButtonClick_videoButton,
    ID_MAIN_settingButton, onButtonClick_settingButton,
    ID_MAIN_appsButton, onButtonClick_appsButton,
    ID_MAIN_ToMusic, onButtonClick_ToMusic,
    ID_MAIN_PrevButton, onButtonClick_PrevButton,
    ID_MAIN_ButtonPlay, onButtonClick_ButtonPlay,
    ID_MAIN_NextButton, onButtonClick_NextButton,
};
/***************/


typedef void (*SeekBarCallback)(ZKSeekBar *pSeekBar, int progress);
typedef struct {
    int id;
    SeekBarCallback callback;
}S_ZKSeekBarCallback;
/*TAG:SeekBarCallbackTab*/
static S_ZKSeekBarCallback SZKSeekBarCallbackTab[] = {
    ID_MAIN_PlayProgressSeekbar, onProgressChanged_PlayProgressSeekbar,
};


typedef int (*ListViewGetItemCountCallback)(const ZKListView *pListView);
typedef void (*ListViewobtainListItemDataCallback)(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index);
typedef void (*ListViewonItemClickCallback)(ZKListView *pListView, int index, int id);
typedef struct {
    int id;
    ListViewGetItemCountCallback getListItemCountCallback;
    ListViewobtainListItemDataCallback obtainListItemDataCallback;
    ListViewonItemClickCallback onItemClickCallback;
}S_ListViewFunctionsCallback;
/*TAG:ListViewFunctionsCallback*/
static S_ListViewFunctionsCallback SListViewFunctionsCallbackTab[] = {
};


typedef void (*SlideWindowItemClickCallback)(ZKSlideWindow *pSlideWindow, int index);
typedef struct {
    int id;
    SlideWindowItemClickCallback onSlideItemClickCallback;
}S_SlideWindowItemClickCallback;
/*TAG:SlideWindowFunctionsCallbackTab*/
static S_SlideWindowItemClickCallback SSlideWindowItemClickCallbackTab[] = {
};


typedef void (*EditTextInputCallback)(const std::string &text);
typedef struct {
    int id;
    EditTextInputCallback onEditTextChangedCallback;
}S_EditTextInputCallback;
/*TAG:EditTextInputCallback*/
static S_EditTextInputCallback SEditTextInputCallbackTab[] = {
};

typedef void (*VideoViewCallback)(ZKVideoView *pVideoView, int msg);
typedef struct {
    int id; //VideoView ID
    bool loop; // 是否是轮播类型
    int defaultvolume;//轮播类型时,默认视频音量
    VideoViewCallback onVideoViewCallback;
}S_VideoViewCallback;
/*TAG:VideoViewCallback*/
static S_VideoViewCallback SVideoViewCallbackTab[] = {
};


typedef void (*RadioGroupCallback)(ZKRadioGroup*, int);
typedef struct {
  int id;
  RadioGroupCallback onCheckedChanged;
}S_RadioGroupCallback;
/*TAG:RadioGroupCallbackTab*/
static S_RadioGroupCallback SRadioGroupCallbackTab[] = {
    ID_MAIN_page3RadioGroup, onCheckedChanged_page3RadioGroup,
    ID_MAIN_StatusRadioGroup, onCheckedChanged_StatusRadioGroup,
};

mainActivity::mainActivity() {
	//todo add init code here
	mVideoLoopIndex = -1;
	mVideoLoopErrorCount = 0;
}

mainActivity::~mainActivity() {
  //todo add init file here
  // 退出应用时需要反注册
    EASYUICONTEXT->unregisterGlobalTouchListener(this);
    unregisterProtocolDataUpdateListener(onProtocolDataUpdate);
    onUI_quit();
    mActivityPtr = NULL;
    mmusicPicTextViewPtr = NULL;
    msettingPage2ButtonPtr = NULL;
    malbumPage2ButtonPtr = NULL;
    mvideoPage2ButtonPtr = NULL;
    mmusicPage2ButtonPtr = NULL;
    mbluetoothPage2ButtonPtr = NULL;
    mmiracastPage2ButtonPtr = NULL;
    maicastPage2ButtonPtr = NULL;
    mairplayPage2ButtonPtr = NULL;
    mandroidautoPage2ButtonPtr = NULL;
    mcarplayPage2ButtonPtr = NULL;
    mpage3RadioGroupPtr = NULL;
    maudiooutputButtonPtr = NULL;
    mWindow4Ptr = NULL;
    mtimeTextViewPtr = NULL;
    mweekTextViewPtr = NULL;
    mdateTextViewPtr = NULL;
    mandroidautoButtonPtr = NULL;
    mcarplayButtonPtr = NULL;
    mairpalyButtonPtr = NULL;
    mvolumeoutputButtonPtr = NULL;
    mbtButtonPtr = NULL;
    mmusicButtonPtr = NULL;
    mvideoButtonPtr = NULL;
    msettingButtonPtr = NULL;
    mappsButtonPtr = NULL;
    mTextView4Ptr = NULL;
    mStatusRadioGroupPtr = NULL;
    mWindow3Ptr = NULL;
    mWindow2Ptr = NULL;
    mPageWindow1Ptr = NULL;
    mTextView6Ptr = NULL;
    mWindow5Ptr = NULL;
    mlinkTipsWindowPtr = NULL;
    mTextView3Ptr = NULL;
    mWindow1Ptr = NULL;
    mbtTipsWindowPtr = NULL;
    martistTextViewPtr = NULL;
    mTextView1Ptr = NULL;
    mToMusicPtr = NULL;
    mPlayProgressSeekbarPtr = NULL;
    mTextView2Ptr = NULL;
    mPrevButtonPtr = NULL;
    mButtonPlayPtr = NULL;
    mNextButtonPtr = NULL;
    mtitleTextViewPtr = NULL;
    mmusicWindowPtr = NULL;
    mTextViewBgPtr = NULL;
}

const char* mainActivity::getAppName() const{
	return "main.ftu";
}

//TAG:onCreate
void mainActivity::onCreate() {
	Activity::onCreate();
    mmusicPicTextViewPtr = (ZKTextView*)findControlByID(ID_MAIN_musicPicTextView);
    msettingPage2ButtonPtr = (ZKButton*)findControlByID(ID_MAIN_settingPage2Button);
    malbumPage2ButtonPtr = (ZKButton*)findControlByID(ID_MAIN_albumPage2Button);
    mvideoPage2ButtonPtr = (ZKButton*)findControlByID(ID_MAIN_videoPage2Button);
    mmusicPage2ButtonPtr = (ZKButton*)findControlByID(ID_MAIN_musicPage2Button);
    mbluetoothPage2ButtonPtr = (ZKButton*)findControlByID(ID_MAIN_bluetoothPage2Button);
    mmiracastPage2ButtonPtr = (ZKButton*)findControlByID(ID_MAIN_miracastPage2Button);
    maicastPage2ButtonPtr = (ZKButton*)findControlByID(ID_MAIN_aicastPage2Button);
    mairplayPage2ButtonPtr = (ZKButton*)findControlByID(ID_MAIN_airplayPage2Button);
    mandroidautoPage2ButtonPtr = (ZKButton*)findControlByID(ID_MAIN_androidautoPage2Button);
    mcarplayPage2ButtonPtr = (ZKButton*)findControlByID(ID_MAIN_carplayPage2Button);
    mpage3RadioGroupPtr = (ZKRadioGroup*)findControlByID(ID_MAIN_page3RadioGroup);if(mpage3RadioGroupPtr!= NULL){mpage3RadioGroupPtr->setCheckedChangeListener(this);}
    maudiooutputButtonPtr = (ZKButton*)findControlByID(ID_MAIN_audiooutputButton);
    mWindow4Ptr = (ZKWindow*)findControlByID(ID_MAIN_Window4);
    mtimeTextViewPtr = (ZKTextView*)findControlByID(ID_MAIN_timeTextView);
    mweekTextViewPtr = (ZKTextView*)findControlByID(ID_MAIN_weekTextView);
    mdateTextViewPtr = (ZKTextView*)findControlByID(ID_MAIN_dateTextView);
    mandroidautoButtonPtr = (ZKButton*)findControlByID(ID_MAIN_androidautoButton);
    mcarplayButtonPtr = (ZKButton*)findControlByID(ID_MAIN_carplayButton);
    mairpalyButtonPtr = (ZKButton*)findControlByID(ID_MAIN_airpalyButton);
    mvolumeoutputButtonPtr = (ZKButton*)findControlByID(ID_MAIN_volumeoutputButton);
    mbtButtonPtr = (ZKButton*)findControlByID(ID_MAIN_btButton);
    mmusicButtonPtr = (ZKButton*)findControlByID(ID_MAIN_musicButton);
    mvideoButtonPtr = (ZKButton*)findControlByID(ID_MAIN_videoButton);
    msettingButtonPtr = (ZKButton*)findControlByID(ID_MAIN_settingButton);
    mappsButtonPtr = (ZKButton*)findControlByID(ID_MAIN_appsButton);
    mTextView4Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView4);
    mStatusRadioGroupPtr = (ZKRadioGroup*)findControlByID(ID_MAIN_StatusRadioGroup);if(mStatusRadioGroupPtr!= NULL){mStatusRadioGroupPtr->setCheckedChangeListener(this);}
//    if(mappSlideWindowPtr!= NULL){
//    }
    mWindow3Ptr = (ZKWindow*)findControlByID(ID_MAIN_Window3);
    mWindow2Ptr = (ZKWindow*)findControlByID(ID_MAIN_Window2);
    mPageWindow1Ptr = (ZKPageWindow*)findControlByID(ID_MAIN_PageWindow1);
    mTextView6Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView6);
    mWindow5Ptr = (ZKWindow*)findControlByID(ID_MAIN_Window5);
    mlinkTipsWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_linkTipsWindow);
    mTextView3Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView3);
    mWindow1Ptr = (ZKWindow*)findControlByID(ID_MAIN_Window1);
    mbtTipsWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_btTipsWindow);
    martistTextViewPtr = (ZKTextView*)findControlByID(ID_MAIN_artistTextView);
    mTextView1Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView1);
    mToMusicPtr = (ZKButton*)findControlByID(ID_MAIN_ToMusic);
    mPlayProgressSeekbarPtr = (ZKSeekBar*)findControlByID(ID_MAIN_PlayProgressSeekbar);if(mPlayProgressSeekbarPtr!= NULL){mPlayProgressSeekbarPtr->setSeekBarChangeListener(this);}
    mTextView2Ptr = (ZKTextView*)findControlByID(ID_MAIN_TextView2);
    mPrevButtonPtr = (ZKButton*)findControlByID(ID_MAIN_PrevButton);
    mButtonPlayPtr = (ZKButton*)findControlByID(ID_MAIN_ButtonPlay);
    mNextButtonPtr = (ZKButton*)findControlByID(ID_MAIN_NextButton);
    mtitleTextViewPtr = (ZKTextView*)findControlByID(ID_MAIN_titleTextView);
    mmusicWindowPtr = (ZKWindow*)findControlByID(ID_MAIN_musicWindow);
    mTextViewBgPtr = (ZKTextView*)findControlByID(ID_MAIN_TextViewBg);
	mActivityPtr = this;
	onUI_init();
  registerProtocolDataUpdateListener(onProtocolDataUpdate);
  rigesterActivityTimer();
}

void mainActivity::onClick(ZKBase *pBase) {
	//TODO: add widget onClik code 
    int buttonTablen = sizeof(sButtonCallbackTab) / sizeof(S_ButtonCallback);
    for (int i = 0; i < buttonTablen; ++i) {
        if (sButtonCallbackTab[i].id == pBase->getID()) {
            if (sButtonCallbackTab[i].callback((ZKButton*)pBase)) {
            	return;
            }
            break;
        }
    }


    int len = sizeof(sAppInfoTab) / sizeof(sAppInfoTab[0]);
    for (int i = 0; i < len; ++i) {
        if (sAppInfoTab[i].id == pBase->getID()) {
            EASYUICONTEXT->openActivity(sAppInfoTab[i].pApp);
            return;
        }
    }

	Activity::onClick(pBase);
}

void mainActivity::onResume() {
	Activity::onResume();
	EASYUICONTEXT->registerGlobalTouchListener(this);
	startVideoLoopPlayback();
	onUI_show();
}

void mainActivity::onPause() {
	Activity::onPause();
	EASYUICONTEXT->unregisterGlobalTouchListener(this);
	stopVideoLoopPlayback();
	onUI_hide();
}

void mainActivity::onIntent(const Intent *intentPtr) {
	Activity::onIntent(intentPtr);
	onUI_intent(intentPtr);
}

bool mainActivity::onTimer(int id) {
	return onUI_Timer(id);
}

void mainActivity::onProgressChanged(ZKSeekBar *pSeekBar, int progress){

    int seekBarTablen = sizeof(SZKSeekBarCallbackTab) / sizeof(S_ZKSeekBarCallback);
    for (int i = 0; i < seekBarTablen; ++i) {
        if (SZKSeekBarCallbackTab[i].id == pSeekBar->getID()) {
            SZKSeekBarCallbackTab[i].callback(pSeekBar, progress);
            break;
        }
    }
}

int mainActivity::getListItemCount(const ZKListView *pListView) const{
    int tablen = sizeof(SListViewFunctionsCallbackTab) / sizeof(S_ListViewFunctionsCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SListViewFunctionsCallbackTab[i].id == pListView->getID()) {
            return SListViewFunctionsCallbackTab[i].getListItemCountCallback(pListView);
            break;
        }
    }
    return 0;
}

void mainActivity::obtainListItemData(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index){
    int tablen = sizeof(SListViewFunctionsCallbackTab) / sizeof(S_ListViewFunctionsCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SListViewFunctionsCallbackTab[i].id == pListView->getID()) {
            SListViewFunctionsCallbackTab[i].obtainListItemDataCallback(pListView, pListItem, index);
            break;
        }
    }
}

void mainActivity::onItemClick(ZKListView *pListView, int index, int id){
    int tablen = sizeof(SListViewFunctionsCallbackTab) / sizeof(S_ListViewFunctionsCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SListViewFunctionsCallbackTab[i].id == pListView->getID()) {
            SListViewFunctionsCallbackTab[i].onItemClickCallback(pListView, index, id);
            break;
        }
    }
}

void mainActivity::onSlideItemClick(ZKSlideWindow *pSlideWindow, int index) {
    int tablen = sizeof(SSlideWindowItemClickCallbackTab) / sizeof(S_SlideWindowItemClickCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SSlideWindowItemClickCallbackTab[i].id == pSlideWindow->getID()) {
            SSlideWindowItemClickCallbackTab[i].onSlideItemClickCallback(pSlideWindow, index);
            break;
        }
    }
}
void mainActivity::onSlidePageChange(ZKSlideWindow *pSlideWindow, int page) {
    // 屏蔽翻页事件 - 什么都不做
        // 直接返回，不执行任何翻页逻辑，从而屏蔽翻页事件
        return;

}

bool mainActivity::onTouchEvent(const MotionEvent &ev) {
    return onmainActivityTouchEvent(ev);
}

void mainActivity::onTextChanged(ZKTextView *pTextView, const std::string &text) {
    int tablen = sizeof(SEditTextInputCallbackTab) / sizeof(S_EditTextInputCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SEditTextInputCallbackTab[i].id == pTextView->getID()) {
            SEditTextInputCallbackTab[i].onEditTextChangedCallback(text);
            break;
        }
    }
}

void mainActivity::rigesterActivityTimer() {
    int tablen = sizeof(REGISTER_ACTIVITY_TIMER_TAB) / sizeof(S_ACTIVITY_TIMEER);
    for (int i = 0; i < tablen; ++i) {
        S_ACTIVITY_TIMEER temp = REGISTER_ACTIVITY_TIMER_TAB[i];
        registerTimer(temp.id, temp.time);
    }
}


void mainActivity::onVideoPlayerMessage(ZKVideoView *pVideoView, int msg) {
    int tablen = sizeof(SVideoViewCallbackTab) / sizeof(S_VideoViewCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SVideoViewCallbackTab[i].id == pVideoView->getID()) {
        	if (SVideoViewCallbackTab[i].loop) {
                //循环播放
        		videoLoopPlayback(pVideoView, msg, i);
        	} else if (SVideoViewCallbackTab[i].onVideoViewCallback != NULL){
        	    SVideoViewCallbackTab[i].onVideoViewCallback(pVideoView, msg);
        	}
            break;
        }
    }
}

void mainActivity::videoLoopPlayback(ZKVideoView *pVideoView, int msg, size_t callbackTabIndex) {

	switch (msg) {
	case ZKVideoView::E_MSGTYPE_VIDEO_PLAY_STARTED:
		LOGD("ZKVideoView::E_MSGTYPE_VIDEO_PLAY_STARTED\n");
    if (callbackTabIndex >= (sizeof(SVideoViewCallbackTab)/sizeof(S_VideoViewCallback))) {
      break;
    }
		pVideoView->setVolume(SVideoViewCallbackTab[callbackTabIndex].defaultvolume / 10.0);
		mVideoLoopErrorCount = 0;
		break;
	case ZKVideoView::E_MSGTYPE_VIDEO_PLAY_ERROR:
		/**错误处理 */
		++mVideoLoopErrorCount;
		if (mVideoLoopErrorCount > 100) {
			LOGD("video loop error counts > 100, quit loop playback !");
            break;
		} //不用break, 继续尝试播放下一个
	case ZKVideoView::E_MSGTYPE_VIDEO_PLAY_COMPLETED:
		LOGD("ZKVideoView::E_MSGTYPE_VIDEO_PLAY_COMPLETED\n");
        std::vector<std::string> videolist;
        std::string fileName(getAppName());
        if (fileName.size() < 4) {
             LOGD("getAppName size < 4, ignore!");
             break;
        }
        fileName = fileName.substr(0, fileName.length() - 4) + "_video_list.txt";
        fileName = "/mnt/extsd/" + fileName;
        if (!parseVideoFileList(fileName.c_str(), videolist)) {
            LOGD("parseVideoFileList failed !");
		    break;
        }
		if (pVideoView && !videolist.empty()) {
			mVideoLoopIndex = (mVideoLoopIndex + 1) % videolist.size();
			pVideoView->play(videolist[mVideoLoopIndex].c_str());
		}
		break;
	}
}

void mainActivity::startVideoLoopPlayback() {
    int tablen = sizeof(SVideoViewCallbackTab) / sizeof(S_VideoViewCallback);
    for (int i = 0; i < tablen; ++i) {
    	if (SVideoViewCallbackTab[i].loop) {
    		ZKVideoView* videoView = (ZKVideoView*)findControlByID(SVideoViewCallbackTab[i].id);
    		if (!videoView) {
    			return;
    		}
    		//循环播放
    		videoLoopPlayback(videoView, ZKVideoView::E_MSGTYPE_VIDEO_PLAY_COMPLETED, i);
    		return;
    	}
    }
}

void mainActivity::stopVideoLoopPlayback() {
    int tablen = sizeof(SVideoViewCallbackTab) / sizeof(S_VideoViewCallback);
    for (int i = 0; i < tablen; ++i) {
    	if (SVideoViewCallbackTab[i].loop) {
    		ZKVideoView* videoView = (ZKVideoView*)findControlByID(SVideoViewCallbackTab[i].id);
    		if (!videoView) {
    			return;
    		}
    		if (videoView->isPlaying()) {
    		    videoView->stop();
    		}
    		return;
    	}
    }
}

bool mainActivity::parseVideoFileList(const char *pFileListPath, std::vector<string>& mediaFileList) {
	mediaFileList.clear();
	if (NULL == pFileListPath || 0 == strlen(pFileListPath)) {
        LOGD("video file list is null!");
		return false;
	}

	ifstream is(pFileListPath, ios_base::in);
	if (!is.is_open()) {
		LOGD("cann't open file %s \n", pFileListPath);
		return false;
	}
	char tmp[1024] = {0};
	while (is.getline(tmp, sizeof(tmp))) {
		string str = tmp;
		removeCharFromString(str, '\"');
		removeCharFromString(str, '\r');
		removeCharFromString(str, '\n');
		if (str.size() > 1) {
     		mediaFileList.push_back(str.c_str());
		}
	}
  LOGD("(f:%s, l:%d) parse fileList[%s], get [%d]files", __FUNCTION__,
      __LINE__, pFileListPath, int(mediaFileList.size()));
  for (std::vector<string>::size_type i = 0; i < mediaFileList.size(); i++) {
    LOGD("file[%u]:[%s]", int(i), mediaFileList[i].c_str());
  }
	is.close();

	return true;
}

int mainActivity::removeCharFromString(string& nString, char c) {
    string::size_type   pos;
    while(1) {
        pos = nString.find(c);
        if(pos != string::npos) {
            nString.erase(pos, 1);
        } else {
            break;
        }
    }
    return (int)nString.size();
}

void mainActivity::registerUserTimer(int id, int time) {
	registerTimer(id, time);
}

void mainActivity::unregisterUserTimer(int id) {
	unregisterTimer(id);
}

void mainActivity::resetUserTimer(int id, int time) {
	resetTimer(id, time);
}
void mainActivity::onCheckedChanged(ZKRadioGroup* pRadioGroup, int checkedID) {
    int tablen = sizeof(SRadioGroupCallbackTab) / sizeof(S_RadioGroupCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SRadioGroupCallbackTab[i].id == pRadioGroup->getID()) {
        	SRadioGroupCallbackTab[i].onCheckedChanged(pRadioGroup, checkedID);
            break;
        }
    }
}