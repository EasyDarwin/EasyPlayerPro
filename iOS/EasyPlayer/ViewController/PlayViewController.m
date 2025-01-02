
#import "PlayViewController.h"
#import <VideoToolbox/VideoToolbox.h>
#import <AudioToolbox/AudioToolbox.h>
#import <CommonCrypto/CommonDigest.h>
#import <QuartzCore/QuartzCore.h>
#import "CustomAlertView.h"
#import "NSUserDefaultsUnit.h"
#import "PathUnit.h"
#import "Masonry.h"
#import <WHToast.h>

#define BottomViewHeight 128

PlayViewController *pvc = nil;

@interface PlayViewController()<UIAlertViewDelegate> {
    NSTimer* _toolbarTimer;
    NSTimer *_fpsTimer;
    
    float speed;
    BOOL _isMediaSliderBeingDragged;
}

@property (nonatomic, assign) BOOL statusBarHidden;

@property (nonatomic, strong) UIButton *backBtn;
@property (nonatomic, strong) UIWebView *webView;

@property (nonatomic, assign) CGRect bottomViewFrame;
@property (nonatomic, strong) UIView *bottomView;
@property (nonatomic, strong) UIButton *slowBtn;
@property (nonatomic, strong) UIButton *fastBtn;
@property (nonatomic, strong) UIButton *playAndStopBtn;
@property (nonatomic, strong) UIButton *forwardBtn;
@property (nonatomic, strong) UIButton *nextBtn;
@property (nonatomic, strong) UILabel *fpsLabel;
@property (nonatomic, strong) UIButton *recordBtn;
@property (nonatomic, strong) UIButton *screenShotBtn;
@property (nonatomic, strong) UIButton *scaleBtn;
@property (nonatomic, strong) UIButton *fullBtn;
@property (nonatomic, strong) UISlider *slider;
@property (nonatomic, strong) UILabel *spendLabel;
@property (nonatomic, strong) UILabel *totalLabel;

@property (nonatomic, strong) UITapGestureRecognizer *tapGesture;
@end

@implementation PlayViewController

#pragma mark - life cycle

- (void)viewDidLoad {
    [super viewDidLoad];
    
    speed = 1.0;
    
    self.view.backgroundColor = [UIColor blackColor];
    
    // 返回按钮
    _backBtn = [UIButton buttonWithType:UIButtonTypeCustom];
    _backBtn.frame = CGRectMake(0, EasyBarHeight, 44, 44);
    _backBtn.backgroundColor = [UIColor clearColor];
    _backBtn.showsTouchWhenHighlighted = YES;
    [_backBtn setBackgroundImage:[UIImage imageNamed:@"BackVideo"] forState:UIControlStateNormal];
    [_backBtn addTarget:self action:@selector(backBtnDidTouch:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:_backBtn];
    
    // 得到图片的路径
    NSString *path = [[NSBundle mainBundle] pathForResource:@"loading" ofType:@"gif"];
    // 将图片转为NSData
    NSData *gifData = [NSData dataWithContentsOfFile:path];
    _webView = [[UIWebView alloc] init];
    [self.view addSubview:_webView];
    [_webView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.height.equalTo(@260);
        make.centerY.equalTo(self.view.mas_centerY);
        make.left.right.equalTo(@0);
    }];
    
    _webView.scalesPageToFit = YES;//自动调整尺寸
    _webView.scrollView.scrollEnabled = NO;//禁止滚动
    _webView.backgroundColor = UIColorFromRGB(0xebebeb);
    _webView.opaque = 0;
    
    [_webView loadData:gifData MIMEType:@"image/gif" textEncodingName:@"UTF-8" baseURL:[NSURL URLWithString:@""]];
    
    // bottomView
    self.bottomViewFrame = CGRectMake(0, EasyScreenHeight - BottomViewHeight, EasyScreenWidth, BottomViewHeight);
    self.bottomView = [[UIView alloc] initWithFrame:self.bottomViewFrame];
    self.bottomView.backgroundColor = [UIColor blackColor];
    [self.view addSubview:self.bottomView];
    
    CGFloat btnHeigth = 40;
    
    // 减速
    _slowBtn = [[UIButton alloc] init];
    [_slowBtn setImage:[UIImage imageNamed:@"slow_click"] forState:UIControlStateHighlighted];
    _slowBtn.titleLabel.font = [UIFont systemFontOfSize:15.0];
    [_slowBtn addTarget:self action:@selector(slowPlay) forControlEvents:UIControlEventTouchUpInside];
    [self.bottomView addSubview:_slowBtn];
    [_slowBtn mas_makeConstraints:^(MASConstraintMaker *make) {
        make.height.equalTo(@(btnHeigth));
        make.left.equalTo(@0);
        make.top.equalTo(@0);
        make.width.equalTo(@(self.bottomViewFrame.size.width / 5));
    }];
    
    // 快退
    _forwardBtn = [[UIButton alloc] init];
    [_forwardBtn setImage:[UIImage imageNamed:@"moveback_click"] forState:UIControlStateHighlighted];
    [_forwardBtn addTarget:self action:@selector(forward) forControlEvents:UIControlEventTouchDown];
    [self.bottomView addSubview:_forwardBtn];
    [_forwardBtn mas_makeConstraints:^(MASConstraintMaker *make) {
        make.height.equalTo(@(btnHeigth));
        make.left.equalTo(self.slowBtn.mas_right);
        make.top.equalTo(@0);
        make.width.equalTo(@(self.bottomViewFrame.size.width / 5));
    }];
    
    // 播放/暂停
    _playAndStopBtn = [[UIButton alloc] init];
    [_playAndStopBtn addTarget:self action:@selector(playAndStop:) forControlEvents:UIControlEventTouchUpInside];
    [self.bottomView addSubview:_playAndStopBtn];
    [_playAndStopBtn mas_makeConstraints:^(MASConstraintMaker *make) {
        make.height.equalTo(@(btnHeigth));
        make.left.equalTo(self.forwardBtn.mas_right);
        make.top.equalTo(@0);
        make.width.equalTo(@(self.bottomViewFrame.size.width / 5));
    }];
    
    // 快进
    _nextBtn = [[UIButton alloc] init];
    [_nextBtn setImage:[UIImage imageNamed:@"forward_click"] forState:UIControlStateHighlighted];
    [_nextBtn addTarget:self action:@selector(next) forControlEvents:UIControlEventTouchDown];
    [self.bottomView addSubview:_nextBtn];
    [_nextBtn mas_makeConstraints:^(MASConstraintMaker *make) {
        make.height.equalTo(@(btnHeigth));
        make.left.equalTo(self.playAndStopBtn.mas_right);
        make.top.equalTo(@0);
        make.width.equalTo(@(self.bottomViewFrame.size.width / 5));
    }];
    
    // 加速
    _fastBtn = [[UIButton alloc] init];
    _fastBtn.titleLabel.font = [UIFont systemFontOfSize:15.0];
    [_fastBtn setImage:[UIImage imageNamed:@"fast_click"] forState:UIControlStateHighlighted];
    [_fastBtn addTarget:self action:@selector(fastPlay) forControlEvents:UIControlEventTouchUpInside];
    [self.bottomView addSubview:_fastBtn];
    [_fastBtn mas_makeConstraints:^(MASConstraintMaker *make) {
        make.height.equalTo(@(btnHeigth));
        make.left.equalTo(self.nextBtn.mas_right);
        make.top.equalTo(@0);
        make.width.equalTo(@(self.bottomViewFrame.size.width / 5));
    }];
    
    _fpsLabel = [[UILabel alloc] init];
    _fpsLabel.text = @"0FPS";
    _fpsLabel.textAlignment = NSTextAlignmentCenter;
    _fpsLabel.font = [UIFont systemFontOfSize:12];
    [self.bottomView addSubview:_fpsLabel];
    [_fpsLabel mas_makeConstraints:^(MASConstraintMaker *make) {
        make.height.equalTo(@(btnHeigth));
        make.left.equalTo(@0);
        make.top.equalTo(@(btnHeigth));
        make.width.equalTo(@(self.bottomViewFrame.size.width / 5));
    }];
    
    // 录像按钮
    _recordBtn = [[UIButton alloc] init];
    [_recordBtn setImage:[UIImage imageNamed:@"videotape_click"] forState:UIControlStateSelected];
    [_recordBtn addTarget:self action:@selector(handleVideo:) forControlEvents:UIControlEventTouchUpInside];
    [self.bottomView addSubview:_recordBtn];
    [_recordBtn mas_makeConstraints:^(MASConstraintMaker *make) {
        make.height.equalTo(@(btnHeigth));
        make.left.equalTo(self.fpsLabel.mas_right);
        make.top.equalTo(@(btnHeigth));
        make.width.equalTo(@(self.bottomViewFrame.size.width / 5));
    }];
    
    // 截图按钮
    _screenShotBtn = [[UIButton alloc] init];
    [_screenShotBtn addTarget:self action:@selector(screenShot) forControlEvents:UIControlEventTouchUpInside];
    [_screenShotBtn setImage:[UIImage imageNamed:@"snapshot_click"] forState:UIControlStateHighlighted];
    [self.bottomView addSubview:_screenShotBtn];
    [_screenShotBtn mas_makeConstraints:^(MASConstraintMaker *make) {
        make.height.equalTo(@(btnHeigth));
        make.left.equalTo(self.recordBtn.mas_right);
        make.top.equalTo(@(btnHeigth));
        make.width.equalTo(@(self.bottomViewFrame.size.width / 5));
    }];
    
    // 缩放按钮
    _scaleBtn = [[UIButton alloc] init];
    [_scaleBtn setImage:[UIImage imageNamed:@"stretch_click"] forState:UIControlStateHighlighted];
    [_scaleBtn addTarget:self action:@selector(zoomEvent) forControlEvents:UIControlEventTouchUpInside];
    [self.bottomView addSubview:_scaleBtn];
    [_scaleBtn mas_makeConstraints:^(MASConstraintMaker *make) {
        make.height.equalTo(@(btnHeigth));
        make.left.equalTo(self.screenShotBtn.mas_right);
        make.top.equalTo(@(btnHeigth));
        make.width.equalTo(@(self.bottomViewFrame.size.width / 5));
    }];
    
    // 全屏按钮
    _fullBtn = [[UIButton alloc] init];
    [_fullBtn setImage:[UIImage imageNamed:@"full"] forState:UIControlStateNormal];
    [_fullBtn setImage:[UIImage imageNamed:@"portraitVideo"] forState:UIControlStateSelected];
    [_fullBtn addTarget:self action:@selector(fullBtnDidTouch:) forControlEvents:UIControlEventTouchUpInside];
    [self.bottomView addSubview:_fullBtn];
    [_fullBtn mas_makeConstraints:^(MASConstraintMaker *make) {
        make.height.equalTo(@(btnHeigth));
        make.left.equalTo(self.scaleBtn.mas_right);
        make.top.equalTo(@(btnHeigth));
        make.width.equalTo(@(self.bottomViewFrame.size.width / 5));
    }];
    
    if (self.isLocal) {
        _recordBtn.hidden = YES;
        _fpsLabel.hidden = YES;
    }
    
    // 显示已播放时间
    _spendLabel = [[UILabel alloc] init];
    _spendLabel.text = @"00:00";
    _spendLabel.font = [UIFont systemFontOfSize:12.0];
    _spendLabel.tag = 1001;
    [self.bottomView addSubview:_spendLabel];
    [_spendLabel mas_makeConstraints:^(MASConstraintMaker *make) {
        make.size.equalTo(@(CGSizeMake(40, 22)));
        make.left.equalTo(@20);
        make.bottom.equalTo(@(-14));
    }];
    
    // 总时间
    _totalLabel = [[UILabel alloc] init];
    _totalLabel.text = @"00:00";
    _totalLabel.textAlignment = NSTextAlignmentRight;
    _totalLabel.font = [UIFont systemFontOfSize:12.0];
    _totalLabel.tag = 1002;
    [self.bottomView addSubview:_totalLabel];
    [_totalLabel mas_makeConstraints:^(MASConstraintMaker *make) {
        make.size.equalTo(@(CGSizeMake(40, 22)));
        make.right.equalTo(@(-20));
        make.centerY.equalTo(self.spendLabel.mas_centerY);
    }];
    
    [self refreshMediaControl];
    
    _slider = [[UISlider alloc] init];
    _slider.minimumTrackTintColor = UIColorFromRGB(0x00b2cd);
    _slider.thumbTintColor = UIColorFromRGB(0x00b2cd);
    [_slider addTarget:self action:@selector(beginDragMediaSlider) forControlEvents:UIControlEventTouchDown];
    [_slider addTarget:self action:@selector(endDragMediaSlider) forControlEvents:UIControlEventTouchCancel];
    [_slider addTarget:self action:@selector(didSliderTouchUpInside) forControlEvents:UIControlEventTouchUpInside];
    [_slider addTarget:self action:@selector(didSliderTouchUpOutside) forControlEvents:UIControlEventTouchUpOutside];
    [_slider addTarget:self action:@selector(continueDragMediaSlider) forControlEvents:UIControlEventValueChanged];
    [self.bottomView addSubview:_slider];
    [_slider mas_makeConstraints:^(MASConstraintMaker *make) {
        make.centerY.equalTo(self.spendLabel.mas_centerY);
        make.left.equalTo(self.spendLabel.mas_right);
        make.right.equalTo(self.totalLabel.mas_left);
    }];
    
    // 点击屏幕
    self.tapGesture = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleTap)];
    self.tapGesture.numberOfTapsRequired = 1;
    [self.view addGestureRecognizer:self.tapGesture];
    
    pvc = self;
    [self startfpsTimer];
    [self btnNormalImage];
    [self play];
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    
    [self installMovieNotificationObservers];
    [self.player prepareToPlay];
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    
    [[UIApplication sharedApplication] setStatusBarHidden:NO];
    self.statusBarHidden = YES;
    [self prefersStatusBarHidden];
    
    [self stopToolbarTimer];
    
    if (_fpsTimer) {
        [_fpsTimer invalidate];
        _fpsTimer = nil;
    }
    
    UIImage *img = [self.player thumbnailImageAtCurrentTime];
    [pvc writeImage:img toFileAtPath:[PathUnit snapshotWithURL:self.urlStr]];
    
    [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(refreshMediaControl) object:nil];
    
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    
    [self.player shutdown];
    [self.player.view removeFromSuperview];
    self.player = nil;
}

#pragma mark - Notification

/* Register observers for the various movie object notifications. */
-(void)installMovieNotificationObservers {
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(loadStateDidChange:)
                                                 name:IJKMPMoviePlayerLoadStateDidChangeNotification
                                               object:_player];
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(moviePlayBackDidFinish:)
                                                 name:IJKMPMoviePlayerPlaybackDidFinishNotification
                                               object:_player];
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(mediaIsPreparedToPlayDidChange:)
                                                 name:IJKMPMediaPlaybackIsPreparedToPlayDidChangeNotification
                                               object:_player];
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(moviePlayBackStateDidChange:)
                                                 name:IJKMPMoviePlayerPlaybackStateDidChangeNotification
                                               object:_player];
}

- (void)loadStateDidChange:(NSNotification*)notification {
    IJKMPMovieLoadState loadState = _player.loadState;
    
    if ((loadState & IJKMPMovieLoadStatePlaythroughOK) != 0) {
        NSLog(@"loadStateDidChange: IJKMPMovieLoadStatePlaythroughOK: %d\n", (int)loadState);
        
        [self hideLoad];
    } else if ((loadState & IJKMPMovieLoadStateStalled) != 0) {
        NSLog(@"loadStateDidChange: IJKMPMovieLoadStateStalled: %d\n", (int)loadState);
//        [self.player stop];[self.player play];[self.mediaControl refreshMediaControl];
    } else {
        NSLog(@"loadStateDidChange: ???: %d\n", (int)loadState);
    }
}

- (void)moviePlayBackDidFinish:(NSNotification*)notification {
    int reason = [[[notification userInfo] valueForKey:IJKMPMoviePlayerPlaybackDidFinishReasonUserInfoKey] intValue];
    switch (reason) {
        case IJKMPMovieFinishReasonPlaybackEnded:
            NSLog(@"playbackStateDidChange: IJKMPMovieFinishReasonPlaybackEnded: %d\n", reason);
            [self hideLoad];
            break;
        case IJKMPMovieFinishReasonUserExited:
            NSLog(@"playbackStateDidChange: IJKMPMovieFinishReasonUserExited: %d\n", reason);
            [self hideLoad];
            break;
        case IJKMPMovieFinishReasonPlaybackError: {
            NSLog(@"playbackStateDidChange: IJKMPMovieFinishReasonPlaybackError: %d\n", reason);
//            [self play];// 断开重连
            
            [self hideLoad];
            [self.view removeGestureRecognizer:self.tapGesture];
            self.bottomView.alpha = 0;

            UIImageView *iv = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"lost"]];
            iv.contentMode = UIViewContentModeScaleAspectFit;
            iv.backgroundColor = UIColorFromRGB(0xebebeb);
            [self.view addSubview:iv];
            [iv mas_makeConstraints:^(MASConstraintMaker *make) {
                make.height.equalTo(@200);
                make.centerY.equalTo(self.view.mas_centerY);
                make.left.right.equalTo(@0);
            }];
        }
            break;
        default:
            NSLog(@"playbackPlayBackDidFinish: ???: %d\n", reason);
            break;
    }
}

- (void)mediaIsPreparedToPlayDidChange:(NSNotification*)notification {
    NSLog(@"mediaIsPreparedToPlayDidChange\n");
}

- (void)moviePlayBackStateDidChange:(NSNotification*)notification {
    switch (_player.playbackState) {
        case IJKMPMoviePlaybackStateStopped: {
            NSLog(@"IJKMPMoviePlayBackStateDidChange %d: stoped", (int)_player.playbackState);
            break;
        }
        case IJKMPMoviePlaybackStatePlaying: {
            NSLog(@"IJKMPMoviePlayBackStateDidChange %d: playing", (int)_player.playbackState);
            break;
        }
        case IJKMPMoviePlaybackStatePaused: {
            NSLog(@"IJKMPMoviePlayBackStateDidChange %d: paused", (int)_player.playbackState);
            break;
        }
        case IJKMPMoviePlaybackStateInterrupted: {
            NSLog(@"IJKMPMoviePlayBackStateDidChange %d: interrupted", (int)_player.playbackState);
            break;
        }
        case IJKMPMoviePlaybackStateSeekingForward:
        case IJKMPMoviePlaybackStateSeekingBackward: {
            NSLog(@"IJKMPMoviePlayBackStateDidChange %d: seeking", (int)_player.playbackState);
            break;
        }
        default: {
            NSLog(@"IJKMPMoviePlayBackStateDidChange %d: unknown", (int)_player.playbackState);
            break;
        }
    }
}


#pragma mark - click event

- (void)backBtnDidTouch:(id)sender {
    [self dismissViewControllerAnimated:YES completion:nil];
}

// 全屏
- (void)fullBtnDidTouch:(id)sender {
    if (!self.fullBtn.selected) {
        [UIView animateWithDuration:0.5 animations:^{
            self.view.transform = CGAffineTransformMakeRotation(M_PI_2);
            self.view.bounds = CGRectMake(0, 0, EasyScreenHeight, EasyScreenWidth);
            
            [[UIApplication sharedApplication] setStatusBarHidden:YES];
            self.statusBarHidden = NO;
            [self prefersStatusBarHidden];
            
            self.backBtn.frame = CGRectMake(EasyBarHeight, self.backBtn.frame.origin.y, 44, 44);
            
            self.bottomViewFrame = CGRectMake(0, EasyScreenWidth - BottomViewHeight, EasyScreenHeight, BottomViewHeight);
            self.bottomView.frame = self.bottomViewFrame;
            self.bottomView.backgroundColor = UIColorFromRGBA(0x000000, 0.4);
            
            self.fullBtn.selected = YES;
            [self updateConstraints];
            [self btnNormalImage2];
        }];
    } else {
        [UIView animateWithDuration:0.5 animations:^{
            self.view.transform = CGAffineTransformIdentity;
            self.view.bounds = CGRectMake(0, 0, EasyScreenWidth, EasyScreenHeight);
            
            [[UIApplication sharedApplication] setStatusBarHidden:NO];
            self.statusBarHidden = YES;
            [self prefersStatusBarHidden];
            
            self.backBtn.frame = CGRectMake(0, self.backBtn.frame.origin.y, 44, 44);
            
            self.bottomViewFrame = CGRectMake(0, EasyScreenHeight - BottomViewHeight, EasyScreenWidth, BottomViewHeight);
            self.bottomView.frame = self.bottomViewFrame;
            self.bottomView.backgroundColor = [UIColor blackColor];
            
            self.fullBtn.selected = NO;
            [self updateConstraints];
            [self btnNormalImage];
        }];
    }
    
    [self restartToolbarTimer];
}

- (void)beginDragMediaSlider {
    _isMediaSliderBeingDragged = YES;
}

- (void)endDragMediaSlider {
    _isMediaSliderBeingDragged = NO;
}

- (void)didSliderTouchUpOutside {
    [self endDragMediaSlider];
}

- (void)didSliderTouchUpInside {
    self.player.currentPlaybackTime = self.slider.value;
    [self endDragMediaSlider];
}

- (void)continueDragMediaSlider {
    [self refreshMediaControl];
}

- (void)playAndStop:(UIButton *)sender {
    sender.selected = !sender.selected;
    if (sender.selected) {
        [self.player pause];
    } else {
        [self.player play];
    }
}

// 截屏
- (void)screenShot {
    UIImage *img = [self.player thumbnailImageAtCurrentTime];
    [pvc writeImage:img toFileAtPath:[PathUnit screenShotWithURL:self.urlStr]];
    
    [WHToast showMessage:@"图片已保存" duration:2 finishHandler:nil];
}

// 录像
- (void)handleVideo:(UIButton *)sender {
    sender.selected = !sender.selected;
    
    if ([self.player isKindOfClass:[IJKFFMoviePlayerController class]]) {
        IJKFFMoviePlayerController *player = self.player;
        
        @try {
            if (sender.selected) {
                [player recordFilePath:(char *)[[PathUnit recordWithURL:self.urlStr] UTF8String] second:60 * 30];
            } else {
                [player recordFilePath:NULL second:0];
            }
        } @catch (NSException *e) {
            NSLog(@"%@", e);
        } @finally {
            
        }
    }
}

// 慢速播放
- (void)slowPlay {
    speed *= 0.5f;
    
    if (speed < 0.25) {
        speed = 0.25;
    }
    
    if ([self.player isKindOfClass:[IJKFFMoviePlayerController class]]) {
        IJKFFMoviePlayerController *player = self.player;
        
        @try {
            player.playbackRate = speed;
        } @catch (NSException *e) {
            NSLog(@"%@", e);
        } @finally {
            
        }
    }
}

// 快速播放
- (void)fastPlay {
    speed *= 2.0f;
    
    if (speed > 4) {
        speed = 4;
    }
    
    if ([self.player isKindOfClass:[IJKFFMoviePlayerController class]]) {
        IJKFFMoviePlayerController *player = self.player;
        player.playbackRate = speed;
    }
}

// 快退
- (void) forward {
    self.player.currentPlaybackTime -= 5;
    [self refreshMediaControl];
}

// 快进
- (void) next {
    self.player.currentPlaybackTime += 5;
    [self refreshMediaControl];
}

- (void)zoomEvent {
    if (self.player.scalingMode == 0) {
        self.player.scalingMode = IJKMPMovieScalingModeAspectFit;
    } else if (self.player.scalingMode == 1) {
        self.player.scalingMode = IJKMPMovieScalingModeAspectFill;
    } else if (self.player.scalingMode == 2) {
        self.player.scalingMode = IJKMPMovieScalingModeFill;
    } else {
        self.player.scalingMode = IJKMPMovieScalingModeNone;
    }
}

#pragma mark - gesture event

- (void)handleTap {
    CGFloat _alpha = 1 - self.backBtn.alpha;
    
    [UIView animateWithDuration:0.1 delay:0.0 options:UIViewAnimationOptionCurveEaseInOut | UIViewAnimationOptionTransitionNone animations:^{
        self.backBtn.alpha = _alpha;
        self.bottomView.alpha = _alpha;
    } completion:nil];
    
    if (_alpha > 0) {
        [self restartToolbarTimer];
    } else {
        [self stopToolbarTimer];
    }
}

#pragma mark - private method

- (void) btnNormalImage {
    [_slowBtn setImage:[UIImage imageNamed:@"slow"] forState:UIControlStateNormal];
    [_forwardBtn setImage:[UIImage imageNamed:@"moveback"] forState:UIControlStateNormal];
    [_playAndStopBtn setImage:[UIImage imageNamed:@"stop"] forState:UIControlStateNormal];
    [_playAndStopBtn setImage:[UIImage imageNamed:@"play"] forState:UIControlStateSelected];
    [_nextBtn setImage:[UIImage imageNamed:@"forward"] forState:UIControlStateNormal];
    [_fastBtn setImage:[UIImage imageNamed:@"fast"] forState:UIControlStateNormal];
    [_recordBtn setImage:[UIImage imageNamed:@"videotape"] forState:UIControlStateNormal];
    [_screenShotBtn setImage:[UIImage imageNamed:@"snapshot"] forState:UIControlStateNormal];
    [_scaleBtn setImage:[UIImage imageNamed:@"stretch"] forState:UIControlStateNormal];
    _fpsLabel.textColor = UIColorFromRGB(0x999999);
    _spendLabel.textColor = UIColorFromRGB(0x999999);
    _totalLabel.textColor = UIColorFromRGB(0x999999);
    _slider.maximumTrackTintColor = UIColorFromRGB(0x999999);
}

- (void) btnNormalImage2 {
    [_slowBtn setImage:[UIImage imageNamed:@"slow-white"] forState:UIControlStateNormal];
    [_forwardBtn setImage:[UIImage imageNamed:@"moveback_white"] forState:UIControlStateNormal];
    [_playAndStopBtn setImage:[UIImage imageNamed:@"stop_white"] forState:UIControlStateNormal];
    [_playAndStopBtn setImage:[UIImage imageNamed:@"play_white"] forState:UIControlStateSelected];
    [_nextBtn setImage:[UIImage imageNamed:@"forward_white"] forState:UIControlStateNormal];
    [_fastBtn setImage:[UIImage imageNamed:@"fast_white"] forState:UIControlStateNormal];
    [_recordBtn setImage:[UIImage imageNamed:@"videotape_white"] forState:UIControlStateNormal];
    [_screenShotBtn setImage:[UIImage imageNamed:@"snapshot_white"] forState:UIControlStateNormal];
    [_scaleBtn setImage:[UIImage imageNamed:@"stretch_white"] forState:UIControlStateNormal];
    _fpsLabel.textColor = UIColorFromRGB(0xffffff);
    _spendLabel.textColor = UIColorFromRGB(0xffffff);
    _totalLabel.textColor = UIColorFromRGB(0xffffff);
    _slider.maximumTrackTintColor = UIColorFromRGB(0xffffff);
}

// 显示FPS参数
- (void)startfpsTimer {
    if ([[NSThread currentThread] isMainThread]) {
        _fpsTimer = [NSTimer scheduledTimerWithTimeInterval:.5f target:self selector:@selector(refreshfpsView) userInfo:nil repeats:YES];
    } else {
        dispatch_async(dispatch_get_main_queue(), ^{
            [self startfpsTimer];
        });
    }
}

- (void)refreshfpsView {
    if ([self.player isKindOfClass:[IJKFFMoviePlayerController class]]) {
        IJKFFMoviePlayerController *player = self.player;
        self.fpsLabel.text = [NSString stringWithFormat:@"%.0fFPS", player.fpsAtOutput];
    }
}

- (void)refreshMediaControl {
    //    IJKFFMoviePlayerController *player = self.player;
    //
    //    UILabel *totalDurationLabel = [_titleView viewWithTag:1002];
    //    NSString *speedStr = [player transferSpeed];
    //    UILabel *speedLabel = (UILabel *)[_titleView viewWithTag:4002];
    //    if ([[NSThread currentThread] isMainThread]) {
    //        speedLabel.text = speedStr;
    //    } else {
    //        dispatch_async(dispatch_get_main_queue(), ^{
    //            speedLabel.text = speedStr;
    //        });
    //    }
    
    NSTimeInterval duration = self.player.duration;
    NSInteger intDuration = duration + 0.5;
    
    if (intDuration > 0) {
        self.slider.maximumValue = duration;
        self.totalLabel.text = [NSString stringWithFormat:@"%02d:%02d", (int)(intDuration / 60), (int)(intDuration % 60)];
    }
    
    NSTimeInterval position;
    if (_isMediaSliderBeingDragged) {
        position = self.slider.value;
    } else {
        position = self.player.currentPlaybackTime;
    }
    
    NSInteger intPosition = position + 0.5;
    if (intDuration > 0) {
        self.slider.value = position;
    } else {
        self.slider.value = 0.0f;
    }
    
    self.spendLabel.text = [NSString stringWithFormat:@"%02d:%02d", (int)(intPosition / 60), (int)(intPosition % 60)];
    [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(refreshMediaControl) object:nil];
    [self performSelector:@selector(refreshMediaControl) withObject:nil afterDelay:0.5];
}

- (void) updateConstraints {
    [self.slowBtn mas_updateConstraints:^(MASConstraintMaker *make) {
        make.width.equalTo(@(self.bottomViewFrame.size.width / 5));
    }];
    [self.forwardBtn mas_updateConstraints:^(MASConstraintMaker *make) {
        make.width.equalTo(@(self.bottomViewFrame.size.width / 5));
    }];
    [self.playAndStopBtn mas_updateConstraints:^(MASConstraintMaker *make) {
        make.width.equalTo(@(self.bottomViewFrame.size.width / 5));
    }];
    [self.nextBtn mas_updateConstraints:^(MASConstraintMaker *make) {
        make.width.equalTo(@(self.bottomViewFrame.size.width / 5));
    }];
    [self.fastBtn mas_updateConstraints:^(MASConstraintMaker *make) {
        make.width.equalTo(@(self.bottomViewFrame.size.width / 5));
    }];
    [self.fpsLabel mas_updateConstraints:^(MASConstraintMaker *make) {
        make.width.equalTo(@(self.bottomViewFrame.size.width / 5));
    }];
    [self.recordBtn mas_updateConstraints:^(MASConstraintMaker *make) {
        make.width.equalTo(@(self.bottomViewFrame.size.width / 5));
    }];
    [self.screenShotBtn mas_updateConstraints:^(MASConstraintMaker *make) {
        make.width.equalTo(@(self.bottomViewFrame.size.width / 5));
    }];
    [self.scaleBtn mas_updateConstraints:^(MASConstraintMaker *make) {
        make.width.equalTo(@(self.bottomViewFrame.size.width / 5));
    }];
    [self.fullBtn mas_updateConstraints:^(MASConstraintMaker *make) {
        make.width.equalTo(@(self.bottomViewFrame.size.width / 5));
    }];
}

- (void)restartToolbarTimer {
    if (!_toolbarTimer) {
        _toolbarTimer = [NSTimer scheduledTimerWithTimeInterval:4.0 target:self selector:@selector(handleTap) userInfo:nil repeats:NO];
    }
    
    [_toolbarTimer setFireDate:[NSDate dateWithTimeIntervalSinceNow:4.]];
}

- (void)stopToolbarTimer {
    if (_toolbarTimer) {
        [_toolbarTimer invalidate];
        _toolbarTimer = nil;
    }
}

- (void) hideLoad {
    _webView.hidden = YES;
}

- (void) play {
    if (self.player) {
        [self.player.view removeFromSuperview];
    }
    
#ifdef DEBUG
    [IJKFFMoviePlayerController setLogReport:YES];
    [IJKFFMoviePlayerController setLogLevel:k_IJK_LOG_DEBUG];
#else
    [IJKFFMoviePlayerController setLogReport:NO];
    [IJKFFMoviePlayerController setLogLevel:k_IJK_LOG_INFO];
#endif
    
    [IJKFFMoviePlayerController checkIfFFmpegVersionMatch:YES];
    
    IJKFFOptions *options = [IJKFFOptions optionsByDefault];
    [options setFormatOptionIntValue:1000000 forKey:@"analyzeduration"]; // 21s
    [options setFormatOptionIntValue:2048 forKey:@"probesize"];// 2048或者204800
    [options setFormatOptionIntValue:0 forKey:@"auto_convert"];
    [options setFormatOptionIntValue:1 forKey:@"reconnect"];
    [options setFormatOptionIntValue:10 forKey:@"timeout"];
    [options setPlayerOptionIntValue:0 forKey:@"packet-buffering"];
    [options setFormatOptionValue:@"nobuffer" forKey:@"fflags"];
    [options setFormatOptionValue:[NSUserDefaultsUnit UDPDesc] forKey:@"rtsp_transport"];
//    [options setFormatOptionIntValue:1 forKey:@"opensles"];
//    [options setFormatOptionIntValue:1 forKey:@"mediacodec"];
//    [options setFormatOptionIntValue:1 forKey:@"mediacodec-auto-rotate"];
//    [options setFormatOptionIntValue:1 forKey:@"mediacodec-handle-resolution-change"];
    
    // RTSP流对应的iformat的是rtsp,rtmp流对应的iformat的是flv,m3u8流对应的iformat的是hls
    if ([[self.urlStr substringToIndex:4] isEqualToString:@"rtmp"]) {
        [options setFormatOptionValue:@"flv" forKey:@"iformat"];
    } else if ([[self.urlStr substringToIndex:4] isEqualToString:@"m3u8"]) {
        [options setFormatOptionValue:@"hls" forKey:@"iformat"];
    } else {
        [options setFormatOptionValue:@"rtsp" forKey:@"iformat"];
    }
    
    NSURL *url = [NSURL URLWithString:self.urlStr];
    self.player = [[IJKFFMoviePlayerController alloc] initWithContentURL:url withOptions:options key:PLAYER_KEY];
    
    if (self.player) {
        self.player.view.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
        self.player.view.frame = self.view.bounds;
        self.player.scalingMode = IJKMPMovieScalingModeAspectFit;
        self.player.shouldAutoplay = YES;
        self.view.autoresizesSubviews = YES;
        [self.view insertSubview:self.player.view atIndex:0];
    } else {
        [[CustomAlertView shareCustimView] showWithCustomWithTitle:@"" andMessage:@"Key不合法或者已过期"];
    }
}

/**
 保存截图

 @param image 图片
 @param aPath 路径
 @return 保存结果
 */
- (BOOL)writeImage:(UIImage *)image toFileAtPath:(NSString*)aPath {
    if ((image == nil) || (aPath == nil) || ([aPath isEqualToString:@""]))
        return NO;
    
    @try {
        NSData *imageData = nil;
        NSString *ext = [aPath pathExtension];
        
        if ([ext isEqualToString:@"png"]){
            imageData = UIImagePNGRepresentation(image);
        } else {
            imageData = UIImageJPEGRepresentation(image,1);
        }
        
        if ((imageData == nil) || ([imageData length] <= 0)){
            NSLog(@"image data is empty");
            return NO;
        }
        
        [imageData writeToFile:aPath atomically:YES];
        return YES;
    } @catch (NSException *e) {
        NSLog(@"create thumbnail exception.");
    }
    
    return NO;
}

#pragma mark - StatusBar

- (BOOL)prefersStatusBarHidden {
    return self.statusBarHidden;
}

@end
