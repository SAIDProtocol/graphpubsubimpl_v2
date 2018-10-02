require(package "hierarchical");

HRC_TEST_ContentName("abc/def/gh", 0, COMP "abc/de");

HRC_TEST_ContentName("abc//gh");

HRC_TEST_ContentName("abc//gh//", 1, COMP "");

HRC_TEST_ContentName("/abc/def/gh/ijklm/", 1, def);

HRC_TEST_ContentName("//abc/def/gh/ijklm/", 2, def);

DriverManager(stop);