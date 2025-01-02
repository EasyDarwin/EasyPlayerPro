//
//  RecordListCell.h
//  Easy
//
//  Created by leo on 2017/11/6.
//  Copyright © 2017年 leo. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface RecordListCell : UITableViewCell

@property (nonatomic, retain) UIImageView *infoIV;

+ (instancetype)cellWithTableView:(UITableView *)tableView;

@end
