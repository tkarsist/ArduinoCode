
/* 
 * Attitude reference system (no heading for a ahrs ;-) )
 */


// Initializing the struct
void init_Gyro1DKalman(struct Gyro1DKalman *filterdata, float Q_angle, float Q_gyro, float R_angle);


// Kalman predict
void ars_predict(struct Gyro1DKalman *filterdata, const float gyro, const float dt);


// Kalman update
float ars_update(struct Gyro1DKalman *filterdata, const float angle_m);

