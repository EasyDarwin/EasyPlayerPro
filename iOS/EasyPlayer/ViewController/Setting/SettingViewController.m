//
//  SettingViewController.m
//  EasyPlayer
//
//  Created by leo on 2017/12/30.
//  Copyright © 2017年 cs. All rights reserved.
//

#import "SettingViewController.h"
#import "NSUserDefaultsUnit.h"

@interface SettingViewController ()

@property (weak, nonatomic) IBOutlet UISwitch *isAudioSwitch;

@end

@implementation SettingViewController

- (instancetype) initWithStoryboard {
    return [[UIStoryboard storyboardWithName:@"Main" bundle:nil] instantiateViewControllerWithIdentifier:@"SettingViewController"];
}

#pragma mark - init

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.navigationItem.title = @"设置";
    
    [_isAudioSwitch setOnTintColor:UIColorFromRGB(SelectBtnColor)];
    _isAudioSwitch.on = [NSUserDefaultsUnit isUDP];
    
    self.tableView.backgroundColor = UIColorFromRGB(0xFFFFFF);
}

#pragma mark - click event

// UDP模式观看视频(默认TCP模式)
- (IBAction)isAudio:(id)sender {
    [NSUserDefaultsUnit setUDP:_isAudioSwitch.on];
}

#pragma mark - UITableViewDelegate

- (void) tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    [tableView deselectRowAtIndexPath:indexPath animated:YES];
}

@end
