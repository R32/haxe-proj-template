package db;

import sys.db.Object;
import sys.db.Types;


class User extends Object{
	public var id:SId;
	public var name:SString<32>;
	public var phoneNumber:SNull<SText>;
}