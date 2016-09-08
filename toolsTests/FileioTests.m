//
//  FileioTests.m
//  tools
//
//  Created by Yutaka Sugawara on 2016/09/08.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

#import <XCTest/XCTest.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>

@interface FileioTests : XCTestCase
{
    int initok;
    char tdir[256];
}

@end

@implementation FileioTests

- (void)setUp {
    [super setUp];
    char *tp;
    initok = 0;
    tp = getenv("TMPDIR");
    if (tp) {
        if (strlen(tp) + 32 < sizeof tdir) {
            strcpy(tdir, tp);
            strcat(tdir, "FileioTestsXXXXX");
            tp = mkdtemp(tdir);
            if (tp) {
                initok = 1;
            } else {
                puts("mkdtemp\n");
            }
        }
    } else {
        puts("TMPDIR\n");
    }
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testReplaceStdin {
    int r;
    FILE *f;
    char b[256];
    if (!initok) {
        puts("init ok is not OK\n");
        XCTAssertTrue(0);
    }

    if (snprintf(b, sizeof b, "%s/in", tdir) >= sizeof b) {
        printf("test error: path too long(%s)\n", tdir);
        return;
    }
    errno = 0;
    f = fopen(b, "w+b");
    if (!f) {
        perror("file open");
        return;
    }
    fputs("message\n", f);
    fflush(f);
    fseek(f, 0, SEEK_SET);
    r = fclose(stdin);
    if (r != 0) {
        fclose(f);
        XCTAssertEqual(0, r);
    }
    stdin = f;
    if (gets(b)) {
        fclose(f);
        XCTAssertTrue(strcmp("message", b) == 0);
    } else {
        puts("read stdin error\n");
        XCTAssertTrue(0);
    }
}

@end
