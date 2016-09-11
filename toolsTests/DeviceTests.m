//
//  DeviceTests.m
//  tools
//
//  Created by Yutaka Sugawara on 2016/09/11.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

#import <XCTest/XCTest.h>
#import <Foundation/Foundation.h>
#include <stdlib.h>

@interface DeviceTests : XCTestCase

@end

@implementation DeviceTests

- (void)setUp {
    [super setUp];
}

- (void)tearDown {
    [super tearDown];
}

- (void)testGetEnv {
    printf("getenv(TMPDIR): %s\n", getenv("TMPDIR"));
    NSLog(@"NSTemporaryDirectory: %@\n", NSTemporaryDirectory());
}

@end
