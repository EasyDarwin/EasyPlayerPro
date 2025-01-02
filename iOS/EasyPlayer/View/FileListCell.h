//
//  FileListCell.h
//  BTG
//
//  Created by leo on 2017/11/6.
//  Copyright © 2017年 leo. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface FileListCell : UITableViewCell

@property (nonatomic, retain) UILabel *label;

+ (instancetype)cellWithTableView:(UITableView *)tableView;

@end
