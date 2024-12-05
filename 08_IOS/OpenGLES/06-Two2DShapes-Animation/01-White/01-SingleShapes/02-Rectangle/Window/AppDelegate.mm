

#import "AppDelegate.h"
#import "ViewController.h"
#import "OGLES.h"

@implementation AppDelegate
{
@private
    UIWindow *window;
    ViewController *viewController;
    GLESView *glesView;
    
}

-(BOOL) application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    // code
    
    // Get the IPhone Screen Rectangle
    CGRect screenRect = [[UIScreen mainScreen]bounds];
    
    // Create Window
    window = [[UIWindow alloc] initWithFrame:screenRect];
    
    // Create ViewController
    viewController = [[ViewController alloc]init];
    
    // set this viewController as window's view viewController
    [window setRootViewController:viewController];
    
    // Create Our Custom View
    glesView = [[GLESView alloc]initWithFrame:screenRect];
    
    // Set this view as view of viewController
    [viewController setView:glesView];
    
    // Decrement the reference Count of view incremented by setView
    [glesView release];
    
    // set focus on this window and make it visible
    [window makeKeyAndVisible];
    
    return YES;
}

-(void) applicationWillResignActive:(UIApplication *)application
{
    // code
    
}

-(void) applicationDidEnterBackground:(UIApplication *)application
{
    // code
    
}

-(void) applicationWillEnterForeground:(UIApplication *)application
{
    // code
    
}

-(void) applicationDidBecomeActive:(UIApplication *)application
{
    // code
    
}

-(void) applicationWillTerminate:(UIApplication *)application
{
    // code
    
}

-(void) dealloc
{
    // code
    [super dealloc];
    [glesView release];
    [viewController release];
    [window release];
    
}

@end

