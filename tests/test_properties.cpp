#include <gtest/gtest.h>
#include <cxxprop/properties.hpp>

#include <string>
#include <iostream>


TEST(TEST_SYSTEM_HPP, GROUP_CREATE_EXIST) {
    cp::Properties p;
    p.createGroups({"document"});
    
    ASSERT_EQ(p.groupExist("document"), true);
};


TEST(TEST_SYSTEM_HPP, INSERT_GET) {
    cp::Properties p;

    p.insert("new_property", "data");

    p.insert({
        {"element.h1", "h11"},
        {"element.h2", "h12"},
        {"attribute.a1", "att1"},
        {"attribute.a2", "att2"},
    });

    ASSERT_EQ(p["new_property"], static_cast<std::string>("data"));

    ASSERT_EQ(p["element.h1"], static_cast<std::string>("h11"));
    ASSERT_EQ(p.get("element.h2"), static_cast<std::string>("h12"));
    
    ASSERT_EQ(p["attribute.a1"], static_cast<std::string>("att1"));
    ASSERT_EQ(p.get("attribute.a2"), static_cast<std::string>("att2"));

};




TEST(TEST_SYSTEM_HPP, GET_BY_GROUP) {
    cp::Properties p;

    p.insert({
        {"element.h1", "h11"},
        {"element.h2", "h12"},
        {"attribute.a1", "att1"},
        {"attribute.a2", "att2"},
    });

    cp::Properties & p_ele = p.getByGroup("element");
    cp::Properties & p_att = p.getByGroup("attribute");

    ASSERT_EQ(p_ele["h1"], static_cast<std::string>("h11"));
    ASSERT_EQ(p_ele.get("h2"), static_cast<std::string>("h12"));
    
    ASSERT_EQ(p_att["a1"], static_cast<std::string>("att1"));
    ASSERT_EQ(p_att.get("a2"), static_cast<std::string>("att2"));
};


TEST(TEST_SYSTEM_HPP, RET) {
    cp::Properties p;
    p.insert("new_property", "data");

    p.insert({
        {"element.h1", "h11"},
        {"element.h2", "h12"},
        {"attribute.a1", "att1"},
        {"attribute.a2", "att2"},
    });

    cp::Properties & p_ele= p.getByGroup("element");

    ASSERT_EQ(p_ele.ret().get("new_property"), static_cast<std::string>("data"));
};


TEST(TEST_SYSTEM_HPP, RET_NUMBER_OF_RETURNS) {
    cp::Properties p;
    p.insert("new_property", "data");

    p.insert({
        {"element.g2.g3.g4.g5.g6.h1", "h11"},
    });

    cp::Properties & p_ele= p.getByGroup("element");

    ASSERT_EQ(p_ele.ret(6).get("new_property"), static_cast<std::string>("data"));
};