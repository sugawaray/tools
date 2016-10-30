//
//  ProcioTests.m
//  tools
//
//  Created by Yutaka Sugawara on 2016/10/22.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

#include <sh.h>
#import <XCTest/XCTest.h>

@interface ProcioTests : XCTestCase
{
struct Procio o;
}

@end

@implementation ProcioTests

- (void)setUp {
    [super setUp];
}

- (void)tearDown {
    [super tearDown];
}

#if 0
- (void)testInit {
    procioinit(&o, (FILE*)1000, (FILE*)1001, (FILE*)1002);
    XCTAssertEqual((FILE*)1000, o.fp[0][0]);
    XCTAssertEqual((FILE*)1000, o.fp[0][1]);
    XCTAssertEqual((FILE*)1001, o.fp[1][0]);
    XCTAssertEqual((FILE*)1001, o.fp[1][1]);
    XCTAssertEqual((FILE*)1002, o.fp[2][0]);
    XCTAssertEqual((FILE*)1002, o.fp[2][1]);
    XCTAssertEqual((FILE*)0, o.redir[0]);
}
#endif

@end
