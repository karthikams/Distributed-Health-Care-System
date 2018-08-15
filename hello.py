from flask import Flask, render_template, request, redirect, url_for, flash
import sqlite3 as sql
import smtplib
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
from email.mime.base import MIMEBase
from email import encoders
#import datetime
import os
#import tablib
from werkzeug.utils import secure_filename
from flask import send_from_directory
from flask_session import Session

UPLOAD_FOLDER = '/home/karthika/Documents/flask_uploads/'
ALLOWED_EXTENSIONS = set(['txt', 'pdf', 'png', 'jpg', 'jpeg', 'gif'])


app = Flask(__name__, template_folder='template')
app.secret_key = "super secret key"
app.config['SESSION_TYPE'] = 'filesystem'
sess = Session()

patient_id  = "global_variable"
user="global user"


@app.route('/')
def main_page():
	print("entered here")
	return render_template('main_page.html')

@app.route('/new_user',methods = ['POST', 'GET'])
def new_user():
	if (request.method == 'POST'):
		return render_template("new_user.html")

@app.route('/insert_patient_data',methods = ['POST', 'GET'])
def insert_patient_data():
	if request.method == 'POST':
		try:
			conn = sql.connect('db_1.0.db')
			print("Opened database successfully")
			p_id = request.form['id_of_user']			# have username field as uname in html code
			ssn = request.form['ssn_of_user']				# have ssn field as myssn in html code
			password = request.form['psw_of_user']			# have password field as pswd in html code
			name = request.form['name_of_user']			# have password field as pswd in html code
			email_id = request.form['email_of_user']			# have password field as pswd in html code

			print("id Value properly received", p_id)
			cur = conn.cursor()	
			cur.execute("INSERT INTO patient (p_id,p_password,p_name,p_email,p_ssn) VALUES (?,?,?,?,?)",(p_id,password,name,email_id,ssn) )
			cur.execute("INSERT INTO login (health_id,username, password) VALUES (?,?,?)", (p_id,name, password))
			conn.commit()
			print("successfully inserted value in database")
			msg = "Your record has been updated in the database."
		except:
			print("error in database connection or access")
			msg = "error in insert operation"
			conn.rollback()
		finally:
			print("closing connection")
			conn.close()
			return render_template('new_user_success.html', msg = msg)		# go back to patient.html and render with the msg block dynamically



@app.route('/existing_user',methods = ['POST', 'GET'])
def existing_user():
	if (request.method == 'POST'):
		return render_template("input.html")

@app.route('/success',methods = ['POST', 'GET'])
def success():
	return redirect(url_for('main_page'))

@app.route('/book_appointment',methods = ['POST', 'GET'])
def book_appointment():
	if (request.method == 'POST'):
		try:
			conn = sql.connect('db_1.0.db')
			cur = conn.cursor()
			cur.execute("select * from doctor")
			rows = cur.fetchall()
		finally:
			return render_template("appointment_date_fix.html", result = rows)

@app.route('/book_appointment_success',methods = ['POST', 'GET'])
def book_appointment_success():
	if (request.method == 'POST'):
		try:
			conn = sql.connect('db_1.0.db')
			cur = conn.cursor()
			print("Opened database successfully")
			
			doctor_id = request.form["d_id"]
			print("the doctor_id is ", doctor_id)
			print("the type of doctor id is ", type(doctor_id))
			the_date = request.form["appointment_date"]
			the_time = request.form["appointment_time"]
			print("the date is %s" % the_date)
			print("the time is %s" % the_time)
			
			valid_date = the_date.split('/', 2)
			print("the date after splitting is ", the_date)
			valid_time = the_time.split(':', 1)
			print("the time after splitting is",  the_time)
			if(len(valid_date) == 3 and len(valid_time) == 2 ):
				the_date = the_date + "," + the_time
				print("the date inside the if condition is ", the_date)
				print("the patient_id is ", patient_id)
				print("the type of patient_id is ", type(patient_id))
				print(" the statement is UPDATE doctor SET d_appointment_time = %s, d_p_id = %s where d_id = %s" % (the_date, patient_id, doctor_id))
				cur.execute("UPDATE doctor SET d_appointment_time = ?, d_p_id = ? where d_id = ?", (the_date, patient_id, doctor_id))
				cur.execute("UPDATE patient SET p_appointment_date = ?, p_d_id = ? where p_id = ?",(the_date, doctor_id, patient_id))
				print("executed the command")
				conn.commit()
				msg = "Ok, your appointment has been booked. Thank you for your continued trust with our hospital."
			else:
				msg = "Please enter a valid date and time"	
			# print("got p_id", p_id)
			# cur.execute("select * from login")
			# cur.execute("UPDATE patient SET p_appointment_date = ? where p_id = ?",('NULL', patient_id))
			# print("executed the command")
			# conn.commit()
			
		except:
			msg = "Sorry, your appointment could not be booked. Please login and try again."
		finally:
			print("closing database connection")
			conn.close()
			return render_template("scan_report_success.html", msg = msg)


@app.route('/cancel_appointment',methods = ['POST', 'GET'])
def cancel_appointment():
	if (request.method == 'POST'):
		try:
			conn = sql.connect('db_1.0.db')
			cur = conn.cursor()
			
			# cur.execute("select * from login")
			cur.execute("select p_appointment_date, p_d_id from patient where p_id= ?",(patient_id,))
			value=cur.fetchall()
			print("Value for appointment is ", value)
			doctor_id = value[0][1]
			if(value[0][0]== 'NULL'):
				msg="Sorry, you dont have any appointments. To book one, login again and try."
			else:

				cur.execute("UPDATE patient SET p_appointment_date = ?, p_d_id = ? where p_id = ?",('NULL', 'NULL', patient_id))
				cur.execute("UPDATE doctor SET d_appointment_time = ?, d_p_id = ? where d_id = ?", ('NULL', 'NULL', doctor_id))
				print("executed the command")
				conn.commit()
				msg = "Ok, your appointment has been cancelled. Please login to book another appointment."
		except:
			msg = "Sorry, your appointment could not be cancelled. Please login and try again."
		finally:
			print("closing database connection")
			conn.close()
			return render_template("scan_report_success.html", msg = msg)


@app.route('/view_appointment',methods = ['POST', 'GET'])
def view_appointment():
	if (request.method == 'POST'):
		try:
			global patient_id
			conn = sql.connect('db_1.0.db')
			cur = conn.cursor()
			cur.execute("select * from patient where p_id= ?",(patient_id,))
			rows = cur.fetchall()
			print("patient_id is: %s", patient_id)
			print(rows)
		finally:
			print("closing database connection")
			conn.close()
			return render_template("view_appointment.html", result = rows)



@app.route('/result',methods = ['POST', 'GET'])
def result():
	if (request.method == 'POST'):
		try:
			conn = sql.connect('db_1.0.db')
			print("Opened database successfully")
			username = request.form['uname']
			global patient_id
			patient_id = username
			print("Username is: %s" % patient_id)
			password = request.form['psw']
			print("password is: %s" % password)
			dict_login = {'patient_id':patient_id, 'password' : password}
			print("created the dictionary")
			cur = conn.cursor()
			print("created the cursor")
			cur.execute("select * from login")
			print("executed the query")
			rows = cur.fetchall()
			print("the type of rows is", type(rows))
			print(rows)
			for row in rows:
				print("the row in consideration is", row)
				if row[0] == patient_id and row[2] == password:
					print("patient_id present in the database")
					if(row[0][0] == 'p'):
						return render_template("patient.html", result = row)
					elif (row[0][0] == 'd'):
						return render_template("doctor.html", result = row)
					elif (row[0][0] == 'i'):
						return render_template("insurance.html", result = row)
					elif (row[0][0] == 'a'):
						return render_template("admin_main.html", result = row)
			print("Username incorrect")
			return redirect(url_for('main_page'))

		except:
			print("error in database connection or access")
		finally:
			print("closing database connection")
			conn.close()



@app.route('/check_insurance',methods = ['POST', 'GET'])
def check_insurance():
	print("Entered the check insurance flask function")
	if (request.method == 'POST'):
		return render_template("check_insurance.html")


@app.route('/check_insurance_result',methods = ['POST', 'GET'])
def check_insurance_result():
	print("Entered the check insurance result flask function")
	if request.method == 'POST':
		try:
			conn = sql.connect('db_1.0.db')
			print("Opened database successfully")
			i_name = request.form['insurance_comp_name']
			print("company name received is", i_name)
			cur = conn.cursor()
			cur.execute("select patient.p_name, patient.p_email, patient.p_ssn from patient, insurance where insurance.i_company_name = ? and insurance.i_p_id = patient.p_id", (i_name,))
			print("executed the function successfully")
			rows = cur.fetchall()
			print("fetched all rows")
			print("length of rows is", len(rows))
			print("type of rows is", type(rows))
			print("type of rows is", rows)
			# check the type here and print accordingly or append to a list and send that to check_insurance_result.html
		except:
			print("error in database connection or access")
		finally:
			print("closing database connection")
			conn.close()
			return render_template("check_insurance_result.html", result = rows)

@app.route('/scan_report',methods = ['POST', 'GET'])
def scan_report():
	if(request.method == 'POST'):
		try:
			print("Python code to send an email should go here")
			fromaddr = "srividhyaprakash029@gmail.com"
			conn = sql.connect('db_1.0.db')
			print("Opened database successfully")
			cur = conn.cursor()
			cur.execute("select patient.p_email, patient.p_scan_report from patient where patient.p_id = ?", (patient_id,))
			returned_list = cur.fetchall()
			print("The email address (toaddr) should come here", returned_list)
			print("the type of toaddr is ", type(returned_list))
			print(returned_list[0][0])
			toaddr = returned_list[0][0]

			# filename = "d4ecb063-7ece-40b5-b92b-a3a1f04ada34-original.jpeg"
			filename = returned_list[0][1]
			print("just the filename fetched from the server is ", filename)
			body = "Your scan report has been attached along with this mail. \
					Hope you liked your stay at our hospital."
			msg = MIMEMultipart()
			msg['From'] = fromaddr
			msg['To'] = toaddr
			msg['Subject'] = "The hospital wishes you a speedy recovery."

			full_path_of_file = UPLOAD_FOLDER + filename
			print("the full path of the file is", full_path_of_file)
			msg.attach(MIMEText(body, 'plain'))
			attachment = open(full_path_of_file, 'rb')
			msg_send = "Your scan report has been successfully sent to you!"
			part = MIMEBase('application', 'octet-stream')
			part.set_payload((attachment).read())
			encoders.encode_base64(part)
			part.add_header('Content-Disposition', "attachment;filename=%s" % filename)
			msg.attach(part)
			# 'your host address', 'your port number'
			server = smtplib.SMTP('smtp.gmail.com', 587)
			# security function used to protect your password
			server.starttls()

			server.login(fromaddr, "007029svp1") # password should be changed here

			msg = msg.as_string()

			# "my email", "email address to send to", "msg"
			server.sendmail(fromaddr, toaddr, msg)
			server.quit()
		except:
			msg_send = "Sorry, your scan report was not found in the database"

		finally:
			print("closing database connection")
			conn.close()
			return render_template("scan_report_success.html", msg = msg_send)

@app.route('/update_patient_info', methods = ['GET', 'POST'])
def update_patient_info():
	return render_template('update_patient_info_page.html')

@app.route('/update_patient_email',methods = ['POST', 'GET'])
def update_patient_email():	
	return render_template('update_patient_email.html')


@app.route('/update_patient_email_success',methods = ['POST', 'GET'])
def update_patient_email_success():	
	if request.method == 'POST':
		try:
			conn = sql.connect('db_1.0.db')
			print("Opened database successfully")
			# p_id = request.form["id_of_user"]
			email = request.form["email_of_user"]
			cur = conn.cursor()
			print("created the cursor")
			cur.execute("UPDATE patient SET p_email = ? where p_id = ?",(email,patient_id))
			conn.commit()
			msg = "Your Email is successfully updated."
		except:
			msg = "Error in updating your Email, login and try again."
		finally:
			return render_template("scan_report_success.html", msg = msg)


@app.route('/update_patient_ssn',methods = ['POST', 'GET'])
def update_patient_ssn():	
	return render_template('update_patient_ssn.html')


@app.route('/update_patient_ssn_success',methods = ['POST', 'GET'])
def update_patient_ssn_success():	
	if request.method == 'POST':
		try:
			conn = sql.connect('db_1.0.db')
			print("Opened database successfully")
			# p_id = request.form["id_of_user"]
			ssn = request.form["ssn_of_user"]
			cur = conn.cursor()
			print("created the cursor")
			cur.execute("UPDATE patient SET p_ssn = ? where p_id = ?",(ssn,patient_id))
			conn.commit()
			msg = "Your SSN is successfully updated."
		except:
			msg = "Error in updating SSN, login and try again."
		finally:
			return render_template("scan_report_success.html", msg = msg)			

@app.route('/update_patient_name',methods = ['POST', 'GET'])
def update_patient_first_name():	
	return render_template('update_patient_name.html')


@app.route('/update_patient_name_success',methods = ['POST', 'GET'])
def update_patient_name_success():	
	if request.method == 'POST':
		try:
			conn = sql.connect('db_1.0.db')
			print("Opened database successfully")
			# p_id = request.form["id_of_user"]
			new_name = request.form["name_of_user"]
			cur = conn.cursor()
			print("created the cursor")
			cur.execute("UPDATE patient SET p_name = ? where p_id = ?",(new_name,patient_id))
			cur.execute("UPDATE login SET username = ? where health_id = ?",(new_name,patient_id))
			conn.commit()
			msg = "Your name has been successfully updated."
		except:
			msg = "Error in updating your name, login and try again."
		finally:
			return render_template("scan_report_success.html", msg = msg)	


app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER
def allowed_file(filename):
	return '.' in filename and \
		   filename.rsplit('.', 1)[1].lower() in ALLOWED_EXTENSIONS


@app.route('/uploadFile', methods=['GET', 'POST'])
def upload_file():
	if request.method == 'POST':
		# pass
		# check if the post request has the file part
		try:
			print("Entering upload_file")
			if 'file' not in request.files:
				flash('No file part')
				return redirect(request.url)
			file = request.files['file']
			print("printing file", file)
			print("printing file.filename", file.filename)
			print("type of file", type(file))
			scan_report_patient_id = request.form["patient_id_file_upload"]

			# if user does not select file, browser also
			# submit a empty part without filename
			if file.filename == '':
				flash('No selected file')
				return redirect(request.url)
			if file and allowed_file(file.filename):
				filename = secure_filename(file.filename)
				file.save(os.path.join(app.config['UPLOAD_FOLDER'], filename))
				conn = sql.connect('db_1.0.db')
				print("Opened database successfully")
				cur = conn.cursor()
				print("created the cursor")
				cur.execute("UPDATE patient set p_scan_report = ? where p_id = ?", (file.filename, scan_report_patient_id))
				conn.commit()
				print("closing database connection")
				conn.close()
				return redirect(url_for('uploaded_file'))
		except:	
			print("Entered the except block in the upload_file function")
			return render_template("uploadFile.html")		
			# pass
	return render_template("uploadFile.html")

@app.route('/uploaded_file')
def uploaded_file():
	print("entering to go back to the main page")
	return redirect(url_for("main_page"))

@app.route('/admin',methods = ['POST', 'GET'])
def admin():	
	if (request.method == 'POST'):
		try:
			conn = sql.connect('db_1.0.db')
			print("Opened database successfully")
			cur = conn.cursor()
			print("created the cursor")
			cur.execute("select * from login")
			rows = cur.fetchall()
			print(rows)
		finally:
			print("closing database connection")
			conn.close()
			return render_template("admin.html", result = rows)

@app.route('/admin_delete',methods = ['POST', 'GET'])
def admin_delete():
	# if (request.method == 'POST'):
	if (request.method == 'POST'):
		try:
			# global user
			conn = sql.connect('db_1.0.db')
			print("Opened database successfully")
			cur = conn.cursor()
			print("created the cursor")
			cur.execute("select * from login")
			rows = cur.fetchall()

			# user = request.form["selecteduser"]
			# print("user:",user)
			print(rows)
		finally:
			print("closing database connection")
			conn.close()
			return render_template("admin_delete.html", result = rows)

@app.route('/cancel_appointment_doctor',methods = ['POST', 'GET'])
def cancel_appointment_doctor():
	if (request.method == 'POST'):
		try:
			conn = sql.connect('db_1.0.db')
			cur = conn.cursor()
			
			# cur.execute("select * from login")
			cur.execute("select d_appointment_time, d_p_id from doctor where d_id= ?",(patient_id,))
			value=cur.fetchall()
			print("Value for appointment is ", value)
			patient_id_in_doctor = value[0][1]

			if(value[0][0]== 'NULL'):
				msg="Sorry doctor, you dont have any appointments."
			else:
				print("patient_id_in_doctor is ", patient_id_in_doctor)
				print(" the type of patient_id_in_doctor is ", type(patient_id_in_doctor))
				print("the doctor id which is nothing but patient_id here is ", patient_id)
				print("the type of  doctor id which is nothing but patient_id here is ", type(patient_id))
				print("the statement is UPDATE doctor SET d_appointment_time = %s, d_p_id = %s where d_id = %s " % ('NULL', 'NULL', patient_id))
				cur.execute("UPDATE doctor SET d_appointment_time = ?, d_p_id = ? where d_id = ?", ('NULL', 'NULL', patient_id))
				cur.execute("UPDATE patient SET p_appointment_date = ?, p_d_id = ? where p_id = ?",('NULL', 'NULL', patient_id_in_doctor))
				
				print("executed the command")
				conn.commit()
				msg = "Ok doctor, your appointment has been cancelled."
		except:
			msg = "Sorry doctor, unknown error occured. Please login and try again."
		finally:
			print("closing database connection")
			conn.close()
			return render_template("scan_report_success.html", msg = msg)



@app.route('/admin_delete_success',methods = ['POST', 'GET'])
def admin_delete_success():
	if (request.method == 'POST'):
		try:
			global user
			conn = sql.connect('db_1.0.db')
			print("Opened database successfully")
			cur = conn.cursor()
			user = request.form["selecteduser"]
			print("user:%s",user)
			print("created the cursor")
			cur.execute("delete from login where health_id= ?",(user,))
	
			if(user[0] == 'p'):
				cur.execute("delete from patient where p_id= ?",(user,))	
			elif (user[0] == 'd'):
				cur.execute("delete from doctor where d_id= ?",(user,))	
			elif (user[0] == 'i'):
				cur.execute("delete from insurance where i_id= ?",(user,))	
			
			conn.commit()
			msg = "Deleted the account, log back in to delete or view account."
		except:
			print("Entering the admin delete success except block")
			msg = "Sorry, either the account doesn't exist or some other error occured.\n Log back in to try again."
		finally:
			print("closing database connection")
			conn.close()
			return render_template("scan_report_success.html", msg = msg) 



if __name__ == '__main__':
	app.secret_key = 'hdwi891y2erhfiuwela'
	app.config['SESSION_TYPE'] = 'filesystem'

	sess.init_app(app)
	app.debug = True
	app.run(host = '127.0.0.1', debug = True)






