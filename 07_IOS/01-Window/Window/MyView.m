

#import "MyView.h"

@implementation MyView
{
    // code
    NSString *string;
    
}

-(id) initWithFrame:(CGRect)frame
{
    // code
    self = [super initWithFrame:frame];
    if (self) {
        
        // Set the BackGround to Black
        [self setBackgroundColor:[UIColor blackColor]];
    
        // Set the string
        string = @"Hello World";
        
        // Single Tap
        //Create Object of singleTapGestureRecognizer
        UITapGestureRecognizer *singleTapGestureRecognizer = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onSingleTap:)];
        
        // Set Number of taps
        [singleTapGestureRecognizer setNumberOfTapsRequired:1];
        
        // Set Number of Fingers
        [singleTapGestureRecognizer setNumberOfTouchesRequired:1];
        
        // Set the delegate as self
        [singleTapGestureRecognizer setDelegate:self];
        
        // Add this gestureRecognizer to the self
        [self addGestureRecognizer:singleTapGestureRecognizer];
        
        // Double Tap
        //Create Object of singleTapGestureRecognizer
        UITapGestureRecognizer *doubleTapGestureRecognizer = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onDoubleTap:)];
        
        // Set Number of taps
        [doubleTapGestureRecognizer setNumberOfTapsRequired:2];
        
        // Set Number of Fingers
        [doubleTapGestureRecognizer setNumberOfTouchesRequired:2];
        
        // Set the delegate as self
        [doubleTapGestureRecognizer setDelegate:self];
        
        // Add this gestureRecognizer to the self
        [self addGestureRecognizer:doubleTapGestureRecognizer];
        
        // double tap should fail for 2 single taps
        [singleTapGestureRecognizer	 requireGestureRecognizerToFail:doubleTapGestureRecognizer];
        
        
        // Swipe
        UISwipeGestureRecognizer *swipeGestureRecognizer = [[UISwipeGestureRecognizer alloc]initWithTarget:self action:@selector(onSwipe:)];
    
        [swipeGestureRecognizer setDelegate:self];
        
        [self addGestureRecognizer:swipeGestureRecognizer];
        
        // LongPress
        UILongPressGestureRecognizer *longPressGestureRecognizer = [[UILongPressGestureRecognizer alloc]initWithTarget:self action:@selector(onLongPress:)];
    
        [longPressGestureRecognizer setDelegate:self];
        
        [self addGestureRecognizer:longPressGestureRecognizer];
        
        
    }
    
    return self;
    
}

-(void) drawRect:(CGRect)rect
{
    // code
    // declare bg color as black
    UIColor *bgColor = [UIColor blackColor];
    
    // set this color
    [bgColor set];

    // fill the window rec/dirtyrectangle with above black color
    UIRectFill(rect);

    //create the font for our screen
    UIFont *stringFont = [UIFont fontWithName:@"Helvetica" size:24 ];

    //crate string color
    UIColor *fgColor = [UIColor greenColor];

    // create dictionary for our string attributes using above two
    NSDictionary *stringDictionary = [NSDictionary dictionaryWithObjectsAndKeys:stringFont,NSFontAttributeName, fgColor,NSForegroundColorAttributeName, nil];

    // create size of the string according to size of font and otehr attributes
    CGSize stringSize = [string sizeWithAttributes:stringDictionary];
    
    // create point for drawing of string
    CGPoint point;
    
    point.x = (rect.size.width/2 - stringSize.width/2);
    point.y = (rect.size.height/2 - stringSize.height/2);

    // draw the string
    [string drawAtPoint:point withAttributes:stringDictionary ];

    
}

-(BOOL) becomeFirstResponder
{
    // code
    return YES;
    
}

-(void) touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    // code
    
}

-(void) onSingleTap:(UITapGestureRecognizer *)gestureRecognizer
{
    // code
    
    string = @"Double Tap !!";
    [self setNeedsDisplay];
    
}

-(void) onDoubleTap:(UITapGestureRecognizer *)gestureRecognizer
{
    // code
    string = @"Double Tap!!";
    [self setNeedsDisplay];
    
}

-(void) onSwipe:(UISwipeGestureRecognizer *)gestureRecognizer
{
    // code
    string = @"Swipe!!";
    [self setNeedsDisplay];
    
}

-(void) onLongPress:(UILongPressGestureRecognizer *)gestureRecognizer
{
    // code
    string = @"Long Press!!";
    [self setNeedsDisplay];
}

-(void) dealloc
{
    // code
    [super dealloc];
    
}


@end
