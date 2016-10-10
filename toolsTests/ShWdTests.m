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
{
char *p;
char b[512];
}


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

- (void)testRootDir {
    p = ios_getcwd(b, sizeof b);
    XCTAssertNotEqual((char*)0, p);
    XCTAssertEqual(0, strcmp("/", b));
}

- (void)testNonRootDir {
    int r1, r2;
    int i;
    char *argv[] = {
        strdup("cd"), strdup("abc"), 0
        };
    r1 = ios_mkdir("abc", S_IRWXU);
    r2 = cd_main(sizeof argv / sizeof argv[0] - 1, argv);
    for (i = 0; i < sizeof argv / sizeof argv[0] - 1; ++i)
        free(argv[i]);
    p = ios_getcwd(b, sizeof b);
    XCTAssertEqual(0, r2);
    XCTAssertNotEqual((char*)0, p);
    XCTAssertEqual(0, strcmp("/abc", b));
}

@end
