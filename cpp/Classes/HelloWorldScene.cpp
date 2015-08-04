#include "HelloWorldScene.h"
#if COCOS2D_VERSION >= 196608
#include "ui/CocosGUI.h"
#endif

USING_NS_CC;
using namespace sdkbox;

template <typename T> std::string tostr(const T& t) { std::ostringstream os; os<<t; return os.str(); }

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    int winWidth = CCDirector::getInstance()->getWinSize().width;
    
    IAP::setDebug(true);
    IAP::setListener(this);
    IAP::init();
    
    CCMenuItemFont::setFontSize(48);
    
    _coinCount = 0;
    
    _txtCoin = Label::create("0", "fonts/Marker Felt.ttf", 32);
    _txtCoin->setAnchorPoint(cocos2d::Vec2(0, 0));
    _txtCoin->setPosition(cocos2d::Vec2(800, 500));
    addChild(_txtCoin);
    
    CCMenuItemFont *item = CCMenuItemFont::create("Load", this, menu_selector(HelloWorld::onRequestIAP));
    item->setPosition(0, 0);
    CCMenuItemFont *item2 = CCMenuItemFont::create("Restore", this, menu_selector(HelloWorld::onRestoreIAP));
    item->setPosition(0, 100);
    
    CCMenu *menu = CCMenu::create(item, item2, NULL);
    menu->setAnchorPoint(cocos2d::Vec2(0, 0));
    menu->setPosition(cocos2d::Vec2(200, 400));
    addChild(menu);
    
    _iapMenu = CCMenu::create();
    _iapMenu->setPosition(winWidth/2, 200);
    addChild(_iapMenu);
    
    //Close menu
    CCMenuItemFont* close_menu_item = CCMenuItemFont::create("Close", this, menu_selector(HelloWorld::menuCloseCallback));
    CCMenu *close_menu = CCMenu::create(close_menu_item, NULL);
    close_menu->setAnchorPoint(cocos2d::Point(0, 0));
    close_menu->setPosition(cocos2d::Point(800, 100));
    addChild(close_menu);
    
    Product test;
    test.name = "remove_ads";
    
    _products.push_back(test);
    updateIAP(_products);
    
    return true;
}

void HelloWorld::onShowAds(cocos2d::Ref *sender)
{
    CCLOG("Show Ads");
}

void HelloWorld::onRequestIAP(cocos2d::Ref* sender)
{
    IAP::refresh();
}

void HelloWorld::onRestoreIAP(cocos2d::Ref* sender)
{
    IAP::restore();
}

void HelloWorld::onIAP(cocos2d::Ref *sender)
{
    auto btn = static_cast<Node*>(sender);
    Product* p = (Product*)btn->getUserData();
    
    CCLOG("Start IAP %s", p->name.c_str());
    IAP::purchase(p->name);
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
    MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
    return;
#endif
    
    Director::getInstance()->end();
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void HelloWorld::onInitialized(bool ok)
{
    CCLOG("%s : %d", __func__, ok);
}

void HelloWorld::onSuccess(const Product &p)
{
    if (p.name == "coin_package") {
        _coinCount += 1000;
        _txtCoin->setString(tostr(_coinCount));
    }
    else if (p.name == "coin_package2") {
        _coinCount += 5000;
        _txtCoin->setString(tostr(_coinCount));
    }
    else if (p.name == "remove_ads") {
        CCLOG("Remove Ads");
    }
    
    CCLOG("Purchase Success: %s", p.id.c_str());
}

void HelloWorld::onFailure(const Product &p, const std::string &msg)
{
    CCLOG("Purchase Failed: %s", msg.c_str());
}

void HelloWorld::onCanceled(const Product &p)
{
    CCLOG("Purchase Canceled: %s", p.id.c_str());
}

void HelloWorld::onRestored(const Product& p)
{
    CCLOG("Purchase Restored: %s", p.name.c_str());
}

void HelloWorld::updateIAP(const std::vector<sdkbox::Product>& products)
{
    //
    _iapMenu->removeAllChildren();
    _products = products;
    
    int posX = 0;
    int posY = 0;
    
    for (int i=0; i < _products.size(); i++)
    {
        CCLOG("IAP: ========= IAP Item =========");
        CCLOG("IAP: Name: %s", _products[i].name.c_str());
        CCLOG("IAP: ID: %s", _products[i].id.c_str());
        CCLOG("IAP: Title: %s", _products[i].title.c_str());
        CCLOG("IAP: Desc: %s", _products[i].description.c_str());
        CCLOG("IAP: Price: %s", _products[i].price.c_str());
        CCLOG("IAP: Price Value: %f", _products[i].priceValue);
        
#if COCOS2D_VERSION >= 196608
        auto btn = ui::Button::create("button.png");
        //        btn->setTitleText(_products[i].name);
        btn->addClickEventListener(CC_CALLBACK_1(HelloWorld::onIAP, this));
#else
        auto btn = CCMenuItemFont::create(_products[i].name.c_str(), CC_CALLBACK_1(HelloWorld::onIAP, this) );
#endif
        btn->setColor(Color3B::WHITE);
        btn->setUserData(&_products[i]);
        btn->setPosition(CCPoint(posX, posY));
        _iapMenu->addChild(btn);
        posY += 50;
    }
}

void HelloWorld::onProductRequestSuccess(const std::vector<Product> &products)
{
    updateIAP(products);
}

void HelloWorld::onProductRequestFailure(const std::string &msg)
{
    CCLOG("Fail to load products");
}

void HelloWorld::onRestoreComplete(bool ok, const std::string &msg)
{
    CCLOG("%s:%d:%s", __func__, ok, msg.data());
}

