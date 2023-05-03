from flask import Flask
from flaskext.mysql import MySQL

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
def home():
	return("Hello World!!")

if __name__ == '__main__':
	app.run(debug=True, host='0.0.0.0')