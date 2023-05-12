from flask import Flask, render_template
import mysql.connector
from flask import Blueprint, request, jsonify, make_response
import json
import pandas as pd


app = Flask(__name__)

db = mysql.connector.connect(
  host="mysql",
  user="root",
  password=open('/secrets/db_root_password.txt').readline(),
  database='Thermostat_Project'
)

@app.route('/', methods=['GET'])
def home():
    sql = """
    SELECT DV.device_id as device_id, DV.device_name as name, DT.temperature as current_temp, DT.event_id as most_recent_event_id
    FROM Devices DV
    LEFT JOIN Device_Data DT ON DV.device_id = DT.device_id
    INNER JOIN (
        SELECT device_id, max(event_id) event_id
        FROM Device_Data
        GROUP BY device_id
        ) b on b.event_id = DT.event_id
    GROUP BY DT.event_id
    """
    df = pd.read_sql(sql, db)

    device_list = [(df.iloc[i]["name"], df.iloc[i]["current_temp"]) for i in range(len(df))]

    return render_template('home.html',  device_list=device_list)
    
    
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

@app.route('/device/')
def device_list():
    sql = '''
    SELECT D.device_id, D.device_name, R.room_name
    FROM Devices D
    JOIN Rooms R ON D.location = R.room_id
    '''
    df = pd.read_sql(sql, db)

    table = [df.iloc[i].tolist() for i in range(len(df))]
    print(table, flush=True)
    return render_template('device_list.html',  
                            table=table)

@app.route('/device/<device_id>/')
def device(device_id):
    sql = '''
    SELECT *
    FROM Devices D
    LEFT JOIN Device_Data DATA ON D.device_id = DATA.device_id
    WHERE D.device_id = "{}"
    '''

    df = pd.read_sql(sql.format(device_id), db)

    return render_template('home.html',  
                            tables=[df.to_html(classes='data')], 
                            titles=df.columns.values)

@app.route('/room/')
def room_list():
    sql = '''
    SELECT * 
    FROM Rooms
    WHERE room_id > 1
    '''
    df = pd.read_sql(sql, db)

    return render_template('home.html',  
                            tables=[df.to_html(classes='data')], 
                            titles=df.columns.values)

@app.route('/room/<room_id>/')
def room(room_id):
    sql = '''
    SELECT * 
    FROM Rooms R
    LEFT JOIN Devices D ON R.room_id = D.location
    WHERE room_id = "{}"
    '''
    df = pd.read_sql(sql.format(room_id), db)

    return render_template('home.html',  
                            tables=[df.to_html(classes='data')], 
                            titles=df.columns.values) 

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
        

def json_from_sql(stmnt):
    cursor = db.cursor()
    cursor.execute(stmnt)

    column_headers = [x[0] for x in cursor.description]

    json_data = []

    theData = cursor.fetchall()

    for row in theData:
        json_data.append(dict(zip(column_headers, row)))

    return json_data


if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0')