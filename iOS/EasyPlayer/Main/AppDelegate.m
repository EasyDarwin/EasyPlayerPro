
#import "AppDelegate.h"
#import "RootViewController.h"
#import "URLUnit.h"
#import "NSUserDefaultsUnit.h"

#import <RTRootNavigationController/RTRootNavigationController.h>
#import <IQKeyboardManager/IQKeyboardManager.h>
#import <Bugly/Bugly.h>
#import <IJKMediaFramework/IJKMediaFramework.h>

@interface AppDelegate ()

@end

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    
    if (![URLUnit urlModels]) {
        URLModel *model = [[URLModel alloc] initDefault];
        model.url = @"rtsp://184.72.239.149/vod/mp4://BigBuckBunny_175k.mov";
        [URLUnit addURLModel:model];
        
        URLModel *model2 = [[URLModel alloc] initDefault];
        model2.url = @"http://www.easydarwin.org/public/video/3/video.m3u8";
        [URLUnit addURLModel:model2];
        
        URLModel *model3 = [[URLModel alloc] initDefault];
        model3.url = @"http://m4.pptvyun.com/pvod/e11a0/ijblO6coKRX6a8NEQgg8LDZcqPY/eyJkbCI6MTUxNjYyNTM3NSwiZXMiOjYwNDgwMCwiaWQiOiIwYTJkbnEtWG82S2VvcTZMNEsyZG9hZmhvNkNjbTY2WXB3IiwidiI6IjEuMCJ9/0a2dnq-Xo6Keoq6L4K2doafho6Ccm66Ypw.mp4";
        [URLUnit addURLModel:model3];
        
        URLModel *model1 = [[URLModel alloc] initDefault];
        model1.url = @"http://112.17.40.12/PLTV/88888888/224/3221226758/1.m3u8";
        [URLUnit addURLModel:model1];
        
        [NSUserDefaultsUnit setUDP:NO];
    }
    
    // key有效时间
    IJKFFOptions *options = [IJKFFOptions optionsByDefault];
    NSURL *url = [NSURL URLWithString:@""];
    IJKFFMoviePlayerController *player = [[IJKFFMoviePlayerController alloc] initWithContentURL:url withOptions:options key:PLAYER_KEY];
    int days = [player activeDays];
    NSLog(@"key有效期：%d", days);
    [NSUserDefaultsUnit setActiveDay:days];
    
    // Bugly
    [Bugly startWithAppId:@"8a4c2e394d"];
    
    // IQKeyboardManager
    IQKeyboardManager *keyboardManager = [IQKeyboardManager sharedManager]; // 获取类库的单例变量
    keyboardManager.enable = YES; // 控制整个功能是否启用
    keyboardManager.shouldResignOnTouchOutside = YES; // 控制点击背景是否收起键盘
    keyboardManager.shouldToolbarUsesTextFieldTintColor = YES; // 控制键盘上的工具条文字颜色是否用户自定义
    keyboardManager.toolbarManageBehaviour = IQAutoToolbarBySubviews; // 有多个输入框时，可以通过点击Toolbar 上的“前一个”“后一个”按钮来实现移动到不同的输入框
    keyboardManager.enableAutoToolbar = YES; // 控制是否显示键盘上的工具条
    keyboardManager.toolbarDoneBarButtonItemText = @"完成";
    keyboardManager.shouldShowToolbarPlaceholder = YES; // 是否显示占位文字
    keyboardManager.placeholderFont = [UIFont boldSystemFontOfSize:17]; // 设置占位文字的字体
    keyboardManager.keyboardDistanceFromTextField = 10.0f; // 输入框距离键盘的距离
    
    // UI逻辑
    CGRect frame = CGRectMake(0, 0, EasyScreenWidth, EasyScreenHeight);
    self.window = [[UIWindow alloc] initWithFrame:frame];
    self.window.backgroundColor = UIColorFromRGB(0xf5f5f5);
    [self.window makeKeyAndVisible];
    
    // 设置UI
    RootViewController *vc = [[RootViewController alloc] initWithStoryboard];
    RTRootNavigationController *rootVC = [[RTRootNavigationController alloc] init];
    [rootVC setViewControllers:@[ vc ]];
    
    self.window.rootViewController = rootVC;
    
    NSString *pname = [[NSProcessInfo processInfo] processName];
    NSLog(@"进程名：%@", pname);
    
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end
