//
//  BrandShopCell.m
//  Easy
//
//  Created by leo on 2017/11/6.
//  Copyright © 2017年 leo. All rights reserved.
//

#import "ScreenShotListCell.h"

@implementation ScreenShotListCell

+ (instancetype)cellWithTableView:(UITableView *)tableView {
    static NSString *identifier = @"ScreenShotListCell";
    // 1.缓存中
    ScreenShotListCell *cell = [tableView dequeueReusableCellWithIdentifier:identifier];
    // 2.创建
    if (cell == nil) {
        cell = [[ScreenShotListCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:identifier];
    }
    
    cell.selectionStyle = UITableViewCellSelectionStyleNone;
    return cell;
}

/**
 *  构造方法(在初始化对象的时候会调用)
 *  一般在这个方法中添加需要显示的子控件
 */
- (instancetype)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier {
    if (self = [super initWithStyle:style reuseIdentifier:reuseIdentifier]) {
        self.backgroundColor = [UIColor whiteColor];
        
//        UIView *lineView = [[UIView alloc] init];
//        lineView.frame = CGRectMake(0, 180 - 5, EasyScreenWidth, 5);
//        lineView.backgroundColor = UIColorFromRGB(0xf2f2f2);
//        [self addSubview:lineView];

        _infoIV = [[UIImageView alloc] init];
        _infoIV.contentMode = UIViewContentModeScaleAspectFit;
        _infoIV.frame = CGRectMake(0, 0, EasyScreenWidth, 200);
        [self addSubview:_infoIV];
    }
    
    return self;
}

@end
