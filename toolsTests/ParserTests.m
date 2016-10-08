//
//  ParserTests.m
//  tools
//
//  Created by Yutaka Sugawara on 2016/09/09.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

#include "sh.h"
#include "decl.h"
#import <XCTest/XCTest.h>

@interface ParserTests : XCTestCase
{
}
@end

@implementation ParserTests

- (void)setUp {
    [super setUp];
    initsh();
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    cleanupsh();
    [super tearDown];
}

- (void)test {
    putsonin("seq 1 10\n");
    procin();
}


@end
