//
//  ShioTests.m
//  tools
//
//  Created by Yutaka Sugawara on 2016/09/11.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

#include "sh.h"
#import <XCTest/XCTest.h>

@interface ShioTests : XCTestCase

@end

@implementation ShioTests

- (void)setUp {
    [super setUp];
}

- (void)tearDown {
    [super tearDown];
}

- (void)testInitsh {
    int r1, r2, r3;
    char b[80];
    r1 = initsh();
    r2 = putsonin("message\n");
    r3 = getsfromin(b, sizeof b);
    cleanupsh();
    XCTAssertEqual(0, r1);
    XCTAssertEqual(0, r2);
    XCTAssertEqual(0, r3);
    XCTAssertEqual(0, strcmp("message\n", b));
}
    
- (void)testIoOut {
    int r1, r2, r3;
    char b[80];
    r1 = initsh();
    r2 = putsonout("message\n");
    r3 = getsfromout(b, sizeof b);
    cleanupsh();
    XCTAssertEqual(0, r1);
    XCTAssertEqual(0, r2);
    XCTAssertEqual(0, r3);
    XCTAssertEqual(0, strcmp("message\n", b));
}

- (void)testIoErr {
    int r1, r2, r3;
    char b[80];
    r1 = initsh();
    r2 = putsonerr("message\n");
    r3 = getsfromerr(b, sizeof b);
    cleanupsh();
    XCTAssertEqual(0, r1);
    XCTAssertEqual(0, r2);
    XCTAssertEqual(0, r3);
    XCTAssertEqual(0, strcmp("message\n", b));
}

@end
