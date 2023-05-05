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

INSERT INTO Devices (device_name, mac_address, location) VALUES ("Device 1", "11:22:33:44:55:66", 1);
INSERT INTO Devices (device_name, mac_address, location) VALUES ("Device 2", "12:23:34:45:56:67", 2);
INSERT INTO Devices (device_name, mac_address, location) VALUES ("Device 3", "10:21:32:43:54:65", 3);

INSERT INTO Device_Data (device_id, date_info, temperature) VALUES (1, '2023-01-01 23:45:15', 21.55);
INSERT INTO Device_Data (device_id, date_info, temperature) VALUES (1, '2023-01-01 23:46:15', 21.56);
INSERT INTO Device_Data (device_id, date_info, temperature) VALUES (1, '2023-01-01 23:47:15', 21.57);
INSERT INTO Device_Data (device_id, date_info, temperature) VALUES (1, '2023-01-01 23:48:15', 21.58);
INSERT INTO Device_Data (device_id, date_info, temperature) VALUES (2, '2023-01-01 23:45:15', 20.55);
INSERT INTO Device_Data (device_id, date_info, temperature) VALUES (2, '2023-01-01 23:46:15', 20.56);
INSERT INTO Device_Data (device_id, date_info, temperature) VALUES (2, '2023-01-01 23:47:15', 20.57);
INSERT INTO Device_Data (device_id, date_info, temperature) VALUES (2, '2023-01-01 23:48:15', 20.58);
INSERT INTO Device_Data (device_id, date_info, temperature) VALUES (3, '2023-01-01 23:45:15', 22.55);
INSERT INTO Device_Data (device_id, date_info, temperature) VALUES (3, '2023-01-01 23:46:15', 22.56);
INSERT INTO Device_Data (device_id, date_info, temperature) VALUES (3, '2023-01-01 23:47:15', 22.57);
INSERT INTO Device_Data (device_id, date_info, temperature) VALUES (3, '2023-01-01 23:48:15', 22.58);

