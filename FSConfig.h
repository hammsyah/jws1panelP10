// ----------------------
// FS CONFIG

int detikiqmh;
int menitiqmh;
uint8_t tipealarm;
uint8_t tmputama;
uint8_t tmpjws;
uint8_t tmpinfo;

struct ConfigJws {
  uint8_t panel; // menit
  uint8_t iqmhs; // menit
  uint8_t iqmhd; // menit
  uint8_t iqmha; // menit
  uint8_t iqmhm; // menit
  uint8_t iqmhi; // menit
  uint8_t durasiadzan; // Menit
  uint8_t ihti; // Koreksi Waktu Menit Jadwal Sholat
  float latitude;
  float longitude;
  int8_t zonawaktu;
  uint8_t hilal;
  char namamasjid[512];
  char informasi[1024];
};

const char *fileconfigjws = "/configjws.json";
ConfigJws configjws;

String message, XML;



// -------------------------------------------
// Membuat file config JWS JSON di File Sistem

void membuatDataAwal() {

  String dataawal = "{\"panel\":\"2\",\"iqmhs\":\"12\",\"iqmhd\":\"8\",\"iqmha\":\"6\",\"iqmhm\":\"5\",\"iqmhi\":\"5\",\"durasiadzan\":\"4\",\"ihti\":\"2\",\"latitude\":\"-6.16\",\"longitude\":\"106.61\",\"zonawaktu\":\"7\",\"hilal\":\"0\",\"namamasjid\":\"UNTUK 1000 MASJID - ELEKTRONMART.COM - 2020\",\"informasi\":\"INFO 1 MASIH KOSONG\"}";

  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, dataawal);

  File configFileJws = LittleFS.open(fileconfigjws, "w");
  
  if (!configFileJws) {
    Serial.println("Gagal membuat file configjws.json untuk ditulis mungkin partisi belum dibuat");
    return;
  }
  
  serializeJson(doc, configFileJws);

  if (error) {
    
    Serial.print(F("deserializeJson() gagal kode sebagai berikut: "));
    Serial.println(error.c_str());
    return;
    
  } else {
    
    configFileJws.close();
    Serial.println("Berhasil membuat file configjws.json");
  
  }  

}



// -------------------------------------------
// Membaca file config JWS JSON di File Sistem

void loadJwsConfig(const char *fileconfigjws, ConfigJws &configjws) {  

  File configFileJws = LittleFS.open(fileconfigjws, "r");

  if (!configFileJws) {
    
    Serial.println("Gagal membuka file configjws.json untuk dibaca");
    membuatDataAwal();
    Serial.println("Sistem restart...");
    ESP.restart();
    
  }

  size_t size = configFileJws.size();
  std::unique_ptr<char[]> buf(new char[size]);
  configFileJws.readBytes(buf.get(), size);

  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, buf.get());  

  if (error) {
    Serial.println("Gagal parse fileconfigjws");
    return;
  }
  
  configjws.panel = doc["panel"];
  configjws.iqmhs = doc["iqmhs"];
  configjws.iqmhd = doc["iqmhd"];
  configjws.iqmha = doc["iqmha"];
  configjws.iqmhm = doc["iqmhm"];
  configjws.iqmhi = doc["iqmhi"];
  configjws.durasiadzan = doc["durasiadzan"];
  configjws.ihti = doc["ihti"];
  configjws.latitude = doc["latitude"];
  configjws.longitude = doc["longitude"];
  configjws.zonawaktu = doc["zonawaktu"];
  configjws.hilal = doc["hilal"];
  strlcpy(configjws.namamasjid, doc["namamasjid"] | "", sizeof(configjws.namamasjid));
  strlcpy(configjws.informasi, doc["informasi"] | "", sizeof(configjws.informasi));

  configFileJws.close(); 

}


// -------------------------------------------
// MEMBACA PARAMETER YANG TERSIMPAN

void bacaParameter() {

  Serial.println(" ");
  Serial.println("PARAMETER TERSIMPAN");
  Serial.print("Jumlah Panel   : "); Serial.println(configjws.panel);
  Serial.print("Iqomah Subuh   : "); Serial.println(configjws.iqmhs);
  Serial.print("Iqomah Dzuhur  : "); Serial.println(configjws.iqmhd);
  Serial.print("Iqomah Ashar   : "); Serial.println(configjws.iqmha);
  Serial.print("Iqomah Maghrib : "); Serial.println(configjws.iqmhm);
  Serial.print("Iqomah Isya    : "); Serial.println(configjws.iqmhi);
  Serial.print("Durasi Adzan   : "); Serial.println(configjws.durasiadzan);
  Serial.print("Ihtiyati       : "); Serial.println(configjws.ihti);
  Serial.print("Latitude       : "); Serial.println(configjws.latitude);
  Serial.print("Longitude      : "); Serial.println(configjws.longitude);
  Serial.print("Zona Waktu     : "); Serial.println(configjws.zonawaktu);
  Serial.print("Derajat Hilal  : "); Serial.println(configjws.hilal);
  Serial.print("Nama Masjid    : "); Serial.println(configjws.namamasjid);
  Serial.print("informasi      : "); Serial.println(configjws.informasi);
  Serial.println(" ");
  lebarpanel = (int)configjws.panel ;
  Serial.print("panel      : "); Serial.println(lebarpanel);
  
}


// PENGHITUNG JADWAL SHOLAT

void JadwalSholat() {

  /*
    CALCULATION METHOD
    ------------------
    Jafari,   // Ithna Ashari
    Karachi,  // University of Islamic Sciences, Karachi
    ISNA,     // Islamic Society of North America (ISNA)
    MWL,      // Muslim World League (MWL)
    Makkah,   // Umm al-Qura, Makkah
    Egypt,    // Egyptian General Authority of Survey
    Custom,   // Custom Setting

    JURISTIC
    --------
    Shafii,    // Shafii (standard)
    Hanafi,    // Hanafi

    ADJUSTING METHOD
    ----------------
    None,        // No adjustment
    MidNight,   // middle of night
    OneSeventh, // 1/7th of night
    AngleBased, // angle/60th of night

    TIME IDS
    --------
    Fajr,
    Sunrise,
    Dhuhr,
    Asr,
    Sunset,
    Maghrib,
    Isha
  
  */

  set_calc_method(Karachi);
  set_asr_method(Shafii);
  set_high_lats_adjust_method(AngleBased);
  set_fajr_angle(20);
  set_isha_angle(18);

  get_prayer_times(rTah, rBul, rTgl, configjws.latitude, configjws.longitude, configjws.zonawaktu, times);

}



// ------------------------------
// UPDATE HISAB TANGGAL

F1kM_Hisab HisabTanggal;

int Hjr_Date, Hjr_Month, Hjr_Year;

void TanggalHijriah() {

  HisabTanggal.setLocationOnEarth(configjws.latitude, configjws.longitude); //Latitude Longitude TANGERANG
  HisabTanggal.setHilalVisibilityFactor(configjws.hilal);
  HisabTanggal.Greg2HijrDate(rTgl,rBul,rTah,Hjr_Date,Hjr_Month,Hjr_Year);

}



//-----------------------------------
// UPDATE WAKTU

void UpdateWaktu() {

  static long pM;
  static uint16_t d;
  

  if (millis() - pM > 1000) {
    d++;
    if (d < 2) {
      BacaRTC();
      JadwalSholat();
      TanggalHijriah();
    } else if (d < 60) { // Update setiap 60 detik
      rDet++;
      if (rDet > 59) {
        rMen++;
        rDet=0;
        if (rMen > 59) {
          rJam++;
          rMen = 0;
        }
      }
    } else {
      d = 0; 
    }
    pM = millis();    
  }
  
}



//----------------------------------------------------------------------
// XML UNTUK JEMBATAN DATA MESIN DENGAN WEB

void XMLWaktu(){

  XML="<?xml version='1.0'?>";
  XML+="<t>";
  
    XML+="<Tahun>";
    XML+=rTah;
    XML+="</Tahun>";
    XML+="<Bulan>";
    XML+=rBul;
    XML+="</Bulan>";
    XML+="<Tanggal>";
    XML+=rTgl;
    XML+="</Tanggal>";
    XML+="<Jam>";
      if(rJam<10){
        XML+="0";
        XML+=rJam;
      }else{XML+=rJam;}
    XML+="</Jam>";
    XML+="<Menit>";
      if(rMen<10){
        XML+="0";
        XML+=rMen;
      }else{XML+=rMen;}
    XML+="</Menit>";
    XML+="<Detik>";
      if(rDet<10){
        XML+="0";
        XML+=rDet;
      }else{XML+=rDet;}
    XML+="</Detik>";
    XML+="<Suhu>";
    XML+= celsius - 2;
    XML+="</Suhu>";
    
  XML+="</t>"; 
}


void XMLDataJWS(){

  XML="<?xml version='1.0'?>";
  XML+="<t>";

    XML+="<IqomahSubuh>";
    XML+= configjws.iqmhs;
    XML+="</IqomahSubuh>";
    XML+="<IqomahDzuhur>";
    XML+= configjws.iqmhd;
    XML+="</IqomahDzuhur>";
    XML+="<IqomahAshar>";
    XML+= configjws.iqmha;
    XML+="</IqomahAshar>";
    XML+="<IqomahMaghrib>";
    XML+= configjws.iqmhm;
    XML+="</IqomahMaghrib>";
    XML+="<IqomahIsya>";
    XML+= configjws.iqmhi;
    XML+="</IqomahIsya>";
    XML+="<DurasiAdzan>";
    XML+= configjws.durasiadzan;
    XML+="</DurasiAdzan>";
    XML+="<Ihtiyati>";
    XML+= configjws.ihti;
    XML+="</Ihtiyati>";
    XML+="<Latitude>";
    XML+= configjws.latitude;
    XML+="</Latitude>";
    XML+="<Longitude>";
    XML+= configjws.longitude;
    XML+="</Longitude>";
    XML+="<ZonaWaktu>";
    XML+= configjws.zonawaktu;
    XML+="</ZonaWaktu>";
    XML+="<Hilal>";
    XML+= configjws.hilal;
    XML+="</Hilal>";
    XML+="<NamaMasjid>";
    XML+= configjws.namamasjid;
    XML+="</NamaMasjid>";
    XML+="<Informasi>";
    XML+= configjws.informasi;
    XML+="</Informasi>";

    XML+="<Panel>";
    XML+= configjws.panel;
    XML+="</Panel>";
    
  XML+="</t>"; 
}


// u_int8_t lebarpanel = 2;
// DMDESP Disp(lebarpanel, 1);  // Jumlah Panel P10 yang digunakan (KOLOM,BARIS)

