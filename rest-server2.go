package main

import (
	"database/sql"
	"encoding/json"
	"fmt"
	"net/http"
	"time"

	_ "github.com/mattn/go-sqlite3"
)

type temperatureReading struct {
	SensorID    string  `json:"sensor_id"`
	Temperature float64 `json:"temperature"`
	Timestamp   string  `json:"timestamp"`
}

type temperatureReading_v2 struct {
	SensorID    string  `json:"sensor_id"`
	Temperature float64 `json:"temperature"`
	Timestamp   string  `json:"timestamp"`
	Uptime      int     `json:"sensor_uptime"`
}

func temperatureHandler(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodPost {
		http.Error(w, "Invalid request method", http.StatusMethodNotAllowed)
		return
	}

	decoder := json.NewDecoder(r.Body)

	// first try to decode with v2 format
	var t temperatureReading_v2
	err := decoder.Decode(&t)
	if err != nil {
		// check if it's old format
		var t_old temperatureReading
		err := decoder.Decode(&t_old)
		if err != nil {
			http.Error(w, "Error decoding request body", http.StatusBadRequest)
			return
		}
		t.SensorID = t_old.SensorID
		t.Temperature = t_old.Temperature
		t.Timestamp = t_old.Timestamp
		t.Uptime = 0
	}

	// Open SQLite database
	db, err := sql.Open("sqlite3", "./temperatures.db")
	if err != nil {
		fmt.Printf("Error opening database")
		return
	}
	defer db.Close()

	// Parse timestamp
	print(t.Timestamp + " " + t.SensorID + " " + "\n")
	timestamp, err := time.Parse(time.RFC3339, t.Timestamp)
	if err != nil {
		fmt.Printf("Error parsing timestamp")
		return
	}

	// Insert temperature reading into database
	_, err = db.Exec("INSERT INTO readings (sensor_id, temperature, timestamp, sensor_uptime) VALUES (?, ?, ?, ?)", t.SensorID, t.Temperature, timestamp, t.Uptime)
	if err != nil {
		fmt.Printf("Error inserting temperature reading into database")
		return
	}

	fmt.Printf("Stored temperature reading from sensor %s: %.2f at %s uptime %d\n", t.SensorID, t.Temperature, t.Timestamp, t.Uptime)
}

func main() {
	http.HandleFunc("/temperature", temperatureHandler)
	http.ListenAndServe(":7894", nil)
}

