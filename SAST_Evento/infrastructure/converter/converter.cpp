﻿#include "converter.h"

//User Converter::dto2User(const UserDto& val){
//    User obj;
//    obj.name = QString::fromStdString(val.name);
//    obj.avatar = QString::fromStdString(val.avatarUrl);
//    obj.location = QString::fromStdString(val.location);
//    obj.email = QString::fromStdString(val.email);
//    obj.followers = val.followers.totalCount;
//    obj.following = val.following.totalCount;
//    obj.login = QString::fromStdString(val.login);
//    obj.bio = QString::fromStdString(val.bio);
//    obj.statusEmoji = QString::fromStdString(val.status.emojiHTML);
//    obj.statusMessage = QString::fromStdString(val.status.message);
//    return obj;
//}

Slide Converter::dto2Slide(const SlideDTO &val)
{
    Slide obj;
    obj.id = val.id;
    obj.title = QString::fromStdString(val.title);
    // obj.link = QString::fromStdString(val.link);
    obj.url = QString::fromStdString(val.url);
    return obj;
}
