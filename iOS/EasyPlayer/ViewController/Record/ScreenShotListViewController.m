//
//  ScreenShotListViewController.m
//  EasyPlayer
//
//  Created by leo on 2017/12/30.
//  Copyright © 2017年 leo. All rights reserved.
//

#import "ScreenShotListViewController.h"
#import "ScreenShotListCell.h"
#import "PathUnit.h"
#import "URLModel.h"
#import "URLUnit.h"

static NSString *collectionCellIdentifier = @"collectionCellIdentifier";

@interface ScreenShotListViewController ()

@property (nonatomic, retain) NSArray *models;

@end

@implementation ScreenShotListViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.navigationItem.title = @"截图记录";
    
    self.tableView.separatorStyle = UITableViewCellSeparatorStyleSingleLine;
    self.tableView.separatorColor = UIColorFromRGB(0xFFFFFF);
    
    _models = [URLUnit urlModels];
    
    for (URLModel *model in _models) {
        model.images = [[NSMutableArray alloc] initWithArray:[PathUnit screenShotListWithURL:model.url]];
    }
}

#pragma mark - UITableViewDatasource

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return _models.count;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    URLModel *model = _models[section];
    return model.images.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    ScreenShotListCell *cell = [ScreenShotListCell cellWithTableView:tableView];
    
    cell.infoIV.backgroundColor = [UIColor blackColor];
    
    URLModel *model = _models[indexPath.section];
    NSString *imageName = model.images[indexPath.row];
    
    NSString *file = [NSString stringWithFormat:@"%@/%@", [PathUnit baseShotPathWithURL:model.url], imageName];
    cell.infoIV.image = [UIImage imageWithContentsOfFile:file];
    
    return cell;
}

#pragma mark - UITableViewDelegate

- (CGFloat) tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath {
    return 200;
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
        UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"是否确定删除此截图?" message:nil preferredStyle:UIAlertControllerStyleAlert];
        UIAlertAction *delete = [UIAlertAction actionWithTitle:@"删除" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
            
            URLModel *model = self.models[indexPath.section];
            NSString *name = model.images[indexPath.row];
            
            [PathUnit deleteBaseShotName:name url:model.url];
            
            [model.images removeObjectAtIndex:indexPath.row];
            [self.tableView reloadData];
        }];
        UIAlertAction *cancel = [UIAlertAction actionWithTitle:@"取消" style:UIAlertActionStyleCancel handler:nil];
        [alert addAction:delete];
        [alert addAction:cancel];
        [self presentViewController:alert animated:YES completion:nil];
    }
}

@end
