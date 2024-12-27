-- Create Devices Table if it does not exist
DO $$ BEGIN
    IF NOT EXISTS (SELECT 1 FROM pg_tables WHERE schemaname = 'public' AND tablename = 'devices') THEN
        CREATE TABLE devices (
            id SERIAL PRIMARY KEY,
            name TEXT NOT NULL UNIQUE,
            location TEXT,
            created_at TIMESTAMP DEFAULT NOW()
        );
    END IF;
END $$;


-- Create Metrics Table if it does not exist
DO $$ BEGIN
    IF NOT EXISTS (SELECT 1 FROM pg_tables WHERE schemaname = 'public' AND tablename = 'metrics') THEN
        CREATE TABLE metrics (
            id SERIAL PRIMARY KEY,
            name TEXT NOT NULL UNIQUE,
            unit TEXT NOT NULL,
            description TEXT,
            created_at TIMESTAMP DEFAULT NOW()
        );
    END IF;
END $$;

-- Create Sensor Types Table if it does not exist
DO $$ BEGIN
    IF NOT EXISTS (SELECT 1 FROM pg_tables WHERE schemaname = 'public' AND tablename = 'sensor_types') THEN
        CREATE TABLE sensor_types (
            id SERIAL PRIMARY KEY,
            name TEXT NOT NULL UNIQUE,
            description TEXT,
            created_at TIMESTAMP DEFAULT NOW()
        );
    END IF;
END $$;

-- Create Sensors Table if it does not exist
DO $$ BEGIN
    IF NOT EXISTS (SELECT 1 FROM pg_tables WHERE schemaname = 'public' AND tablename = 'sensors') THEN
        CREATE TABLE sensors (
            id SERIAL PRIMARY KEY,
            device_id INT NOT NULL REFERENCES devices(id),
            name TEXT NOT NULL UNIQUE,
            type INT NOT NULL REFERENCES sensor_types(id),
            location TEXT,
            status BOOLEAN DEFAULT TRUE,
            created_at TIMESTAMP DEFAULT NOW()
        );
    END IF;
END $$;

-- Create Sensor Readings Table if it does not exist
DO $$ BEGIN
    IF NOT EXISTS (SELECT 1 FROM pg_tables WHERE schemaname = 'public' AND tablename = 'sensor_readings') THEN
        CREATE TABLE sensor_readings (
            id SERIAL PRIMARY KEY,
            device_id INT NOT NULL REFERENCES devices(id),
            metric_id INT NOT NULL REFERENCES metrics(id),
            sensor_id INT NOT NULL REFERENCES sensors(id),
            value NUMERIC NOT NULL,
            timestamp TIMESTAMP NOT NULL
        );
    END IF;
END $$;

-- Create RBI Reports Table if it does not exist
DO $$ BEGIN
    IF NOT EXISTS (SELECT 1 FROM pg_tables WHERE schemaname = 'public' AND tablename = 'rbi_reports') THEN
        CREATE TABLE rbi_reports (
            id SERIAL PRIMARY KEY,
            device_id INT NOT NULL REFERENCES sensors(id),
            risk_index NUMERIC NOT NULL,
            generated_at TIMESTAMP NOT NULL DEFAULT NOW()
        );
    END IF;
END $$;

-- Insert Devices if not exists
DO $$ BEGIN
    IF NOT EXISTS (SELECT 1 FROM devices WHERE name = 'Device_A') THEN
        INSERT INTO devices (name, location) VALUES
        ('Device_A', 'Zone A'),
        ('Device_B', 'Zone B');
    END IF;
END $$;

-- Insert Metric Types if not exists
DO $$ BEGIN
    IF NOT EXISTS (SELECT 1 FROM metrics WHERE name = 'temperature') THEN
        INSERT INTO metrics (name, unit, description) VALUES
        ('temperature', '°C', 'Measures ambient temperature'),
        ('pressure', 'Pa', 'Measures atmospheric pressure');
    END IF;
END $$;

-- Insert Sensor Types if not exists
DO $$ BEGIN
    IF NOT EXISTS (SELECT 1 FROM sensor_types WHERE name = 'Temperature Sensor') THEN
        INSERT INTO sensor_types (name, description) VALUES
        ('Temperature Sensor', 'Measures temperature'),
        ('Pressure Sensor', 'Measures atmospheric pressure');
    END IF;
END $$;

-- Insert Sensors if not exists
DO $$ BEGIN
    IF NOT EXISTS (SELECT 1 FROM sensors WHERE name = 'temp_sensor_1') THEN
        INSERT INTO sensors (name, type, location, device_id) VALUES
        ('temp_sensor_1', 1, 'Zone A', 1),
        ('press_sensor_1', 2, 'Zone A', 1),
        ('temp_sensor_2', 1, 'Zone B', 2),
        ('press_sensor_2', 2, 'Zone B', 2);
    END IF;
END $$;
