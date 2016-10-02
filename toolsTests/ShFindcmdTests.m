//
//  ShFindcmdTests.m
//  tools
//
//  Created by Yutaka Sugawara on 2016/10/02.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

#include "sh.h"
#import <XCTest/XCTest.h>

@interface ShFindcmdTests : XCTestCase

@end

@implementation ShFindcmdTests

- (void)setUp {
    [super setUp];
}

- (void)tearDown {
    [super tearDown];
}

- (void)testFind {
    extern int seq_main(int, char**);
    const struct Shcmd *p;
    p = findcmd("seq");
    XCTAssertEqual(0, strcmp(p->name, "seq"));
}

@end
