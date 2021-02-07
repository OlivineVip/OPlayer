//
// Created by Administrator on 2021/2/5.
//

#include "BaseDecode.h"

BaseDecode::BaseDecode() {

}

BaseDecode::~BaseDecode() {

}
//简单

int BaseDecode::init() {
    avformat_network_init();

    avFormatContext = avformat_alloc_context();


    return 0;
}
