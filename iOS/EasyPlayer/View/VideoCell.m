
#import "VideoCell.h"
#import "Masonry.h"

@implementation VideoCell

- (id)initWithFrame:(CGRect)frame {
    if(self = [super initWithFrame:frame]) {
        self.layer.masksToBounds = NO;
        self.layer.contentsScale = [UIScreen mainScreen].scale;
        self.layer.borderColor = [UIColorFromRGB(EasyBaseFontColor) CGColor];
        self.layer.borderWidth = 0.5;
        self.layer.shadowOpacity = 0.5;
        self.layer.shadowColor = [UIColorFromRGB(EasyBaseFontColor) CGColor];
        self.layer.shadowRadius = 5;
        self.layer.shadowOffset  = CGSizeZero;
        self.layer.shadowPath = [UIBezierPath bezierPathWithRect:self.bounds].CGPath;
        
        self.backgroundColor = UIColorFromRGBA(0x000000, 0.4);
        
        self.imageView = [[UIImageView alloc] init];
        self.imageView.image = [UIImage imageNamed:@"ImagePlaceholder"];
        self.imageView.backgroundColor = UIColorFromRGBA(0xffffff, 0.3);
        [self.contentView addSubview:self.imageView];
        [self.imageView mas_makeConstraints:^(MASConstraintMaker *make) {
            make.left.top.equalTo(@0);
            make.width.equalTo(@(self.contentView.frame.size.width));
            make.height.equalTo(@(self.contentView.frame.size.height - VIDEO_TITLE_HEIGHT));
        }];
        
        self.titleLabel = [[UILabel alloc] init];
        self.titleLabel.font = [UIFont systemFontOfSize:15];
        self.titleLabel.textColor = UIColorFromRGB(0xFFFFFF);
        [self bringSubviewToFront:self.titleLabel];
        [self.contentView addSubview:self.titleLabel];
        [self.titleLabel mas_makeConstraints:^(MASConstraintMaker *make) {
            make.left.equalTo(@10);
            make.bottom.equalTo(@0);
            make.right.equalTo(@(-10));
            make.height.equalTo(@(VIDEO_TITLE_HEIGHT));
        }];
    }
    
    return self;
}

@end
