//
//  ShWdTests.m
//  tools
//
//  Created by Yutaka Sugawara on 2016/10/10.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

#include <sh.h>
#include <unistd.h>
#import <XCTest/XCTest.h>

@interface ShWdTests : XCTestCase

@end

@implementation ShWdTests

- (void)setUp {
    [super setUp];
    initsh();
}

- (void)tearDown {
    cleanupsh();
    [super tearDown];
}

- (void)testInitialWD {
    char *p;
    char b[256];
    p = ios_getcwd(b, sizeof b);
    XCTAssertNotEqual((char*)0, p);
    XCTAssertEqual(0, strcmp("/", b));
}

@end
