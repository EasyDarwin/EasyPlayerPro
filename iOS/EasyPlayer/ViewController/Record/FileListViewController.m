//
//  FileListViewController.m
//  EasyPlayer
//
//  Created by yingengyue on 2017/9/15.
//  Copyright © 2017年 duowan. All rights reserved.
//

#import "FileListViewController.h"
#import "RecordListViewController.h"
#import "ScreenShotListViewController.h"
#import "FileListCell.h"

@interface FileListViewController () {
    NSArray *fileNameArray;
}

@end

@implementation FileListViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    self.navigationItem.title = @"文件";
    
    self.tableView.separatorStyle = UITableViewCellSeparatorStyleNone;
    
    fileNameArray = @[ @"录像", @"截图" ];
}

#pragma mark - UITableViewDataSource

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return fileNameArray.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    FileListCell *cell = [FileListCell cellWithTableView:tableView];
    cell.label.text = fileNameArray[indexPath.row];
    
    return cell;
}

#pragma mark - UITableViewDelegate

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath {
    return 50;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    [tableView deselectRowAtIndexPath:indexPath animated:YES];
    
    if (indexPath.row == 0) {
        RecordListViewController *easyVc = [[RecordListViewController alloc] init];
        [self.navigationController pushViewController:easyVc animated:YES];
    } else {
        ScreenShotListViewController *easyVc = [[ScreenShotListViewController alloc] init];
        [self.navigationController pushViewController:easyVc animated:YES];
    }
}

@end
