CREATE DATABASE Thermostat_Project;

USE Thermostat_Project;

CREATE TABLE Rooms (
	room_id			INTEGER			NOT NULL PRIMARY KEY AUTO_INCREMENT,
	room_name		VARCHAR(128)	NOT NULL
);

INSERT INTO Rooms (room_name) VALUES ('UNASSIGNED');

CREATE TABLE Devices (
	device_id 		INTEGER 		NOT NULL PRIMARY KEY AUTO_INCREMENT,
	mac_address		VARCHAR(17)		NOT NULL UNIQUE,
	device_name 	VARCHAR(64) 	NOT NULL DEFAULT "UNNAMED",
	location 		INTEGER			DEFAULT 1,
	CONSTRAINT device_room FOREIGN KEY (location) REFERENCES Rooms(room_id)
		ON UPDATE CASCADE
		ON DELETE RESTRICT
);

CREATE TABLE Device_Data (
	event_id		INTEGER 		NOT NULL PRIMARY KEY AUTO_INCREMENT,
	device_id 		INTEGER			NOT NULL,
	date_info		DATETIME		NOT NULL,
	temperature 	FLOAT			NOT NULL,
	CONSTRAINT data_device FOREIGN KEY (device_id) REFERENCES Devices(device_id)
		ON UPDATE CASCADE
		ON DELETE RESTRICT
);

INSERT INTO Rooms (room_name) VALUES ('Living Room');
INSERT INTO Rooms (room_name) VALUES ('Bedroom');
INSERT INTO Rooms (room_name) VALUES ('Master Bedroom');


