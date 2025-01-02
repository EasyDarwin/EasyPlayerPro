//
//  RecordListViewController.m
//  EasyPlayer
//
//  Created by leo on 2017/12/30.
//  Copyright © 2017年 cs. All rights reserved.
//

#import "RecordListViewController.h"
#import "PlayViewController.h"
#import "RecordListCell.h"
#import "PathUnit.h"
#import "URLModel.h"
#import "URLUnit.h"

@interface RecordListViewController ()

@property (nonatomic, retain) NSArray *models;

@end

@implementation RecordListViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.navigationItem.title = @"录像记录";
    
    self.tableView.separatorStyle = UITableViewCellSeparatorStyleSingleLine;
    self.tableView.separatorColor = UIColorFromRGB(0xFFFFFF);
    
    _models = [URLUnit urlModels];
    
    for (URLModel *model in _models) {
        model.records = [[NSMutableArray alloc] initWithArray:[PathUnit recordListWithURL:model.url]];
    }
}

#pragma mark - UITableViewDatasource

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return _models.count;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    URLModel *model = _models[section];
    return model.records.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    RecordListCell *cell = [RecordListCell cellWithTableView:tableView];
    
    URLModel *model = _models[indexPath.section];
    
    NSString *path = [PathUnit snapshotWithURL:model.url];
    if([[NSFileManager defaultManager] fileExistsAtPath:path]) {
        cell.infoIV.image = [UIImage imageWithContentsOfFile:path];
    } else {
        cell.infoIV.image = [UIImage imageNamed:@"ImagePlaceholder"];
    }
    
    return cell;
}

#pragma mark - UITableViewDelegate

- (CGFloat) tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath {
    return 160;
}

- (void) tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    URLModel *model = _models[indexPath.section];
    NSString *name = model.records[indexPath.row];
    
    NSString *file = [NSString stringWithFormat:@"%@/%@", [PathUnit baseRecordPathWithURL:model.url], name];
    
    PlayViewController *play = [[PlayViewController alloc] init];
    play.urlStr = file;
    play.isLocal = YES;
    [self presentViewController:play animated:YES completion:nil];
}

#pragma mark - 侧滑删除

- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath {
    return YES;
}

-(NSString *)tableView:(UITableView *)tableView titleForDeleteConfirmationButtonForRowAtIndexPath:(NSIndexPath *)indexPath {
    return @"删除";
}

-(void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath {
    // 只要实现这个方法，就实现了默认滑动删除
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"是否确定删除此录像?" message:nil preferredStyle:UIAlertControllerStyleAlert];
        UIAlertAction *delete = [UIAlertAction actionWithTitle:@"删除" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
            
            URLModel *model = self.models[indexPath.section];
            NSString *name = model.records[indexPath.row];
            
            [PathUnit deleteBaseRecordName:name url:model.url];
            
            [model.records removeObjectAtIndex:indexPath.row];
            [self.tableView reloadData];
        }];
        UIAlertAction *cancel = [UIAlertAction actionWithTitle:@"取消" style:UIAlertActionStyleCancel handler:nil];
        [alert addAction:delete];
        [alert addAction:cancel];
        [self presentViewController:alert animated:YES completion:nil];
    }
}

@end
