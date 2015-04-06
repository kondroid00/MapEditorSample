#include "AppDelegate.h"
#include "MainLayer.h"
#include "Screen.h"

USING_NS_CC;


typedef struct tagResolution
{
    cocos2d::Size size;
    char directory[100];
}Resolution;

static Resolution iPhone4Resolution     = { cocos2d::Size(640, 960),   "iPhone4"     };
static Resolution iPhone5Resolution     = { cocos2d::Size(640, 1136),  "iPhone5"     };
static Resolution iPhone6Resolution     = { cocos2d::Size(750, 1334),  "iPhone6"     };
static Resolution iPhone6PlusResolution = { cocos2d::Size(1242, 2208), "iPhone6Plus" };
static Resolution iPadResolution        = { cocos2d::Size(1536, 2048), "iPad"        };


AppDelegate::AppDelegate(){}


AppDelegate::~AppDelegate(){}


//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}


bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLViewImpl::create("My Game");
        director->setOpenGLView(glview);
    }

    auto frameSize = glview->getFrameSize();
    
    std::vector<std::string> searchPath;
    
    glview->setDesignResolutionSize(iPhone6Resolution.size.width, iPhone6Resolution.size.height, ResolutionPolicy::SHOW_ALL);
    
    // iOS MultiResolution Setting
    if (frameSize.width > iPhone6PlusResolution.size.width)
    {
        searchPath.push_back(iPadResolution.directory);
        director->setContentScaleFactor(iPadResolution.size.width/iPhone6Resolution.size.width);
    }
    else if (frameSize.width > iPhone6Resolution.size.width)
    {
        searchPath.push_back(iPhone6PlusResolution.directory);
        director->setContentScaleFactor(iPhone6PlusResolution.size.width/iPhone6Resolution.size.width);
    }
    else if (frameSize.width > iPhone5Resolution.size.width)
    {
        searchPath.push_back(iPhone6Resolution.directory);
        director->setContentScaleFactor(iPhone6Resolution.size.width/iPhone6Resolution.size.width);
    }
    else if (frameSize.height > iPhone4Resolution.size.height)
    {
        searchPath.push_back(iPhone5Resolution.directory);
        director->setContentScaleFactor(iPhone5Resolution.size.width/iPhone6Resolution.size.width);
    }
    else
    {
        searchPath.push_back(iPhone4Resolution.directory);
        director->setContentScaleFactor(iPhone4Resolution.size.width/iPhone5Resolution.size.width);
    }

    // iOS Setting end
    
    FileUtils::getInstance()->setSearchPaths(searchPath);
    
    // turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60.0f);

    // create a scene. it's an autorelease object
    auto scene = MainLayer::createScene();

    // run
    director->runWithScene(scene);

    return true;
}


// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    //SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}


// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    //SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}




