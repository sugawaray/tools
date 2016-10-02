//
//  ShTests.m
//  tools
//
//  Created by Yutaka Sugawara on 2016/10/02.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

#include "sh.h"
#import <XCTest/XCTest.h>

@interface ShTests : XCTestCase {

    struct Wl *wl;
    char **argv;
    int argc;
}
    
@end

@implementation ShTests

- (void)setUp {
    [super setUp];
    wl = 0;
    argv = 0;
    argc = 0;
}

- (void)tearDown {
    int i;
    struct Wl *p, *q;
    for (i = 0, p = wl; i < argc; p = q, ++i) {
        q = p->next;
        free(p->p);
        free(p);
    }
    for (i = 0; i < argc; ++i)
        free(argv[i]);
    free(argv);
    [super tearDown];
}

- (void)testAWord {
    int r;
    wl = malloc(sizeof(struct Wl));
    wl->next = wl;
    wl->p = strdup("word");
    r = genargv(wl, wl, &argv, &argc);
    XCTAssertEqual(0, r);
    XCTAssertEqual(1, argc);
    XCTAssertEqual(0, strcmp("word", argv[0]));
    XCTAssertEqual((char*)0, argv[1]);
}

- (void)testWords {
    int r;
    wl = malloc(sizeof(struct Wl));
    wl->next = malloc(sizeof(struct Wl));
    wl->next->next = malloc(sizeof(struct Wl));
    wl->next->next->next = wl;
    wl->next->next->p = strdup("3rd");
    wl->next->p = strdup("2nd");
    wl->p = strdup("1st");
    r = genargv(wl, wl, &argv, &argc);
    XCTAssertEqual(0, r);
    XCTAssertEqual(3, argc);
    XCTAssertEqual(0, strcmp("1st", argv[0]));
    XCTAssertEqual(0, strcmp("2nd", argv[1]));
    XCTAssertEqual(0, strcmp("3rd", argv[2]));
    XCTAssertEqual((char*)0, argv[3]);
}

@end
