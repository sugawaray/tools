//
//  Fixt1.swift
//  tools
//
//  Created by Yutaka Sugawara on 2016/11/05.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

import Foundation

func copyToDocDir(name: String, ofType: String, inDirectory: String) -> Bool {
    var b: Bundle? = nil
    for x in Bundle.allBundles {
        if (x.bundleIdentifier == nil) {
            continue
        }
        if (x.bundleIdentifier!.hasSuffix(".toolsTests")) {
            b = x
            break
        }
    }
    if (b == nil) {
        return false
    }
    let url = b!.url(forResource: name, withExtension: ofType, subdirectory: inDirectory)
    if (url == nil) {
        return false
    }
    let fm = FileManager.default
    var dst = fm.urls(for: FileManager.SearchPathDirectory.documentDirectory, in: FileManager.SearchPathDomainMask.userDomainMask)[0]
    dst.appendPathComponent(name + "." + ofType)
    do {
        try fm.removeItem(at:dst)
    } catch let e as NSError {
    }
    do {
        try fm.copyItem(at: url!, to: dst)
    } catch let e as NSError {
        return false
    }
    return true
}
