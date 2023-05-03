from flask import Flask
from flaskext.mysql import MySQL
from flask import Blueprint, request, jsonify, make_response
import json

db = MySQL()

app = Flask(__name__)

# these are for the DB object to be able to connect to MySQL. 
app.config['MYSQL_DATABASE_USER'] = 'root'
app.config['MYSQL_DATABASE_PASSWORD'] = open('/secrets/db_root_password.txt').readline()
app.config['MYSQL_DATABASE_HOST'] = 'mysql'
app.config['MYSQL_DATABASE_PORT'] = 3306
app.config['MYSQL_DATABASE_DB'] = 'Thermostat_Project'  # Change this to your DB name

# Initialize the database object with the settings above. 
db.init_app(app)

@app.route('/')
def get_books():
    # get a cursor object from the database
    cursor = db.get_db().cursor()

    sql = """
    SELECT * FROM Device_Data;
    """

    # use cursor to get all data
    cursor.execute(sql)

    # grab the column headers from the returned data
    column_headers = [x[0] for x in cursor.description]

    # create an empty dictionary object to use in 
    # putting column headers together with data
    json_data = []

    # fetch all the data from the cursor
    theData = cursor.fetchall()

    # for each of the rows, zip the data elements together with
    # the column headers. 
    for row in theData:
        json_data.append(dict(zip(column_headers, row)))

    return jsonify(json_data)

	
if __name__ == '__main__':
	app.run(debug=True, host='0.0.0.0')