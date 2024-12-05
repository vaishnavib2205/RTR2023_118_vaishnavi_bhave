

#import "AppDelegate.h"
#import "ViewController.h"
#import "MyView.h"

@implementation AppDelegate
{
@private
    UIWindow *window;
    ViewController *viewController;
    MyView *myView;
    
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
    myView = [[MyView alloc]initWithFrame:screenRect];
    
    // Set this view as view of viewController
    [viewController setView:myView];
    
    // Decrement the reference Count of view incremented by setView
    [myView release];
    
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
    [myView release];
    [viewController release];
    [window release];
    
}

@end

