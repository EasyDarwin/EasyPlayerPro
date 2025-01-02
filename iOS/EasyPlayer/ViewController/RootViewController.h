
#import "BaseViewController.h"
#import "URLModel.h"

/**
 视频广场
 */
@interface RootViewController : BaseViewController

@property (weak, nonatomic) IBOutlet UICollectionView *collectionView;

@property (weak, nonatomic) IBOutlet UIView *bottomView;
@property (weak, nonatomic) IBOutlet NSLayoutConstraint *bottomViewHeight;

@property (weak, nonatomic) IBOutlet UIButton *pushBtn;
@property (weak, nonatomic) IBOutlet UIButton *recordBtn;
@property (weak, nonatomic) IBOutlet UIButton *settingBtn;

@property (nonatomic, strong) NSMutableArray *dataArray;

@property (nonatomic, copy) void (^previewMore)(URLModel *model);

- (instancetype) initWithStoryboard;

@end
