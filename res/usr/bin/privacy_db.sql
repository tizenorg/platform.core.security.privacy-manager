PRAGMA foreign_keys = ON; BEGIN TRANSACTION;

CREATE TABLE PackageInfo(
    UNIQUE_ID INTEGER PRIMARY KEY AUTOINCREMENT,
    PKG_ID TEXT not null,
    IS_SET BOOL not null,
CHECK(1) );

CREATE TABLE Privacy(
    ID INTEGER not null,
    PRIVACY_ID TEXT not null,
    IS_ENABLED INTEGER not null,
    FOREIGN KEY(ID) REFERENCES PackageInfo(UNQUE_ID),
CHECK(1) );

CREATE VIEW PrivacyView as SELECT 
    PackageInfo.UNIQUE_ID, 
    PackageInfo.PKG_ID, 
    Privacy.PRIVACY_ID, 
    Privacy.IS_ENABLED 
    FROM PackageInfo, Privacy 
    WHERE PackageInfo.UNIQUE_ID = Privacy.ID;
COMMIT;
BEGIN TRANSACTION; 
CREATE TABLE DB_VERSION_0_1 (version INT); COMMIT;
