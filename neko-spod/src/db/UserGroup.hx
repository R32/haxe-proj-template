package db;

import sys.db.Object;

@:id(gid, uid)
class UserGroup extends Object{
	@:relation(uid) public var user:User;
	@:relation(gid) public var group:Group;
}