//
//  LexTests.m
//  tools
//
//  Created by Yutaka Sugawara on 2016/09/09.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

#include "cmd.h"
#include "decl.h"
#import <XCTest/XCTest.h>

@interface LexTests : XCTestCase
{
    FILE *f;
    char path[256];
}
@end

@implementation LexTests

- (void)setUp {
    [super setUp];
    int r;
    f = 0;
    path[0] = '\0';
    r = mktestdir("LexTests", path, sizeof path);
    if (r != 0) {
        fprintf(stderr, "test error: mktestdir\n");
        XCTAssert(0);
    }
    if (strlen(path) + 32 >= sizeof path) {
        fprintf(stderr, "test error: test directory path is too long.\n");
        XCTAssert(0);
    }
    strcat(path, "/in");
    r = replacein(path, &f);
    if (r != 0) {
        fprintf(stderr, "test error: replacein failed\n");
        XCTAssert(0);
    }
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
    if (f) {
        fclose(f);
        f = 0;
    }
}

- (void)test {
    fprintf(f, "ls .\n");
    fflush(f);
    fseek(f, 0, SEEK_SET);
    yylex();
}


@end
