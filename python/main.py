from flask import Flask
import mysql.connector
from flask import Blueprint, request, jsonify, make_response
import json


app = Flask(__name__)

db = mysql.connector.connect(
  host="mysql",
  user="root",
  password=open('/secrets/db_root_password.txt').readline(),
  database='Thermostat_Project'
)


@app.route('/', methods=['GET'])
def home():
    # get a cursor object from the database
    cursor = db.cursor()

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

@app.route('/', methods=['POST'])
def add_data():

    # get data from the POST request
    data = request.get_json()
    print(request)
    # create tuple of values from request
    values = (data["device_id"], data["date_info"], data["temperature"])
    # SQL statement to insert data into database
    # event_id field is autopopulated by the SQL server on entry
    sql = '''
    INSERT INTO Device_Data (device_id, date_info, temperature)
    VALUES (%s, %s, %s)
    '''
    try:
        # insert data into database
        cursor = db.cursor()
        cursor.execute(sql, values)
        db.commit()
        return jsonify({"msg":"success"})
    except mysql.connector.IntegrityError as err:
        return jsonify({"error":"UNKNOWN_DEVICE_ID"})

    

@app.route('/new_device/<mac>/', methods=['GET'])
def add_device(mac):
    values = (mac,)
    insert_stmnt = '''
    INSERT INTO Devices (mac_address)
    VALUES (%s)
    '''
    
    select_stmnt = f'''
    SELECT device_id
    FROM Devices
    WHERE mac_address = "{mac}"
    '''
    cursor = db.cursor()
    cursor.execute(select_stmnt.format(mac))
    result = cursor.fetchall()
    if len(result) != 0:
        device_id = result[0][0]
        print(f"Existing Device: {device_id} found with MAC: {mac}", flush=True)
        return jsonify({"device_id":device_id})
    else:
        try:
            cursor.execute(insert_stmnt, values)
            db.commit()
            cursor.execute(select_stmnt.format(mac))
            device_id = cursor.fetchall()[0][0]
            print(f"New device: {device_id} added with MAC: {mac}", flush=True)
            return jsonify({"device_id":device_id})
        except Exception as err:
            print(err)
            return jsonify({"error":str(err)})
        

    


if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0')