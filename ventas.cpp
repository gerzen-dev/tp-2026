#include <iostream>
#include <cstring>
#include <cstdio>
using namespace std;

struct comandas_historicas{
    char fecha[11];
    char nombreMozo[50];
    int codigoProducto;
    int cantidad;
    float comision;
};
struct producto{
    int codigo;
    char descripcion[50];
    float precio;
    int stockActual;
};
struct mozo{
    int idMozo;
    char nombre[50];
    char password[20];
    float totalComision;
};
struct comanda{
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};

const int MAX_MOZOS = 50;
const int MAX_PROD  = 100;
const int MAX_DIA   = 1000;
const int INTENTOS  = 3;
const int CLAVE     = 7;        // las claves de mozos.dat estan desplazadas +7 por caracter
const float PORC_COMISION = 0.10f;

bool entradaCerrada = false;    // se pone en true si se corta la entrada (Ctrl+D / fin de archivo)

// ---------------------------------------------------------------- entrada
void leerLinea(const char* msg, char* buf, int n){
    cout << msg;
    if(!cin.getline(buf, n)){
        if(cin.eof()){ entradaCerrada = true; buf[0] = '\0'; return; }
        cin.clear();                    // linea demasiado larga: se descarta el resto
        cin.ignore(10000, '\n');
        buf[0] = '\0';
    }
}

bool leerEntero(const char* msg, int &valor){
    char buf[64], extra;
    leerLinea(msg, buf, sizeof(buf));
    if(entradaCerrada) return false;
    return sscanf(buf, "%d %c", &valor, &extra) == 1;
}

// ---------------------------------------------------------------- utilidades
void cifrar(const char* origen, char* destino){
    int i = 0;
    for(; origen[i] != '\0'; i++)
        destino[i] = (char)((unsigned char)origen[i] + CLAVE);
    destino[i] = '\0';
}

bool fechaValida(int d, int m, int a){
    if(a < 2000 || a > 2100 || m < 1 || m > 12 || d < 1) return false;
    int dias[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if(m == 2 && ((a % 4 == 0 && a % 100 != 0) || a % 400 == 0)) dias[1] = 29;
    return d <= dias[m-1];
}

void pedirFecha(char* fecha){
    char buf[64], extra;
    int d, m, a;
    while(!entradaCerrada){
        leerLinea("Fecha de hoy (dd-mm-aaaa): ", buf, sizeof(buf));
        if(entradaCerrada) return;
        if(sscanf(buf, "%d-%d-%d %c", &d, &m, &a, &extra) == 3 && fechaValida(d, m, a)){
            sprintf(fecha, "%02d-%02d-%04d", d, m, a);
            return;
        }
        cout << "Fecha invalida. Use el formato dd-mm-aaaa.\n";
    }
}

// ---------------------------------------------------------------- archivos
int cargarMozos(mozo* m){
    FILE* f = fopen("mozos.dat", "rb");
    if(!f) return 0;
    int n = 0;
    // mozos.dat guarda por registro: idMozo(4) + nombre(50) + password(20) = 74 bytes.
    // Se lee campo por campo (no con sizeof(mozo)) porque el archivo no trae totalComision.
    while(n < MAX_MOZOS &&
          fread(&m[n].idMozo, sizeof(int), 1, f) == 1 &&
          fread(m[n].nombre, sizeof(m[n].nombre), 1, f) == 1 &&
          fread(m[n].password, sizeof(m[n].password), 1, f) == 1){
        m[n].totalComision = 0;
        n++;
    }
    fclose(f);
    return n;
}

int cargarInventario(producto* p){
    FILE* f = fopen("inventario.dat", "rb");
    if(!f) return 0;
    int n = (int)fread(p, sizeof(producto), MAX_PROD, f);
    fclose(f);
    return n;
}

bool guardarInventario(producto* p, int n){
    FILE* f = fopen("inventario.dat", "wb");
    if(!f) return false;
    fwrite(p, sizeof(producto), n, f);
    fclose(f);
    return true;
}

// Formato de comandas_dd-mm-aaaa.dat: registros de struct comanda
// (idMozo, codigoProducto, cantidad, comision = 16 bytes). La fecha esta en el nombre del archivo.
int buscarProducto(producto* p, int n, int codigo);

int cargarDia(const char* archivo, comanda* dia, producto* p, int nP, bool &reparado){
    FILE* f = fopen(archivo, "ab");     // lo crea si no existe
    if(f) fclose(f);
    f = fopen(archivo, "rb");
    if(!f) return 0;
    int n = 0;
    reparado = false;
    while(n < MAX_DIA){
        comanda c;
        if(fread(&c.idMozo, sizeof(int), 1, f) != 1) break;
        if(fread(&c.codigoProducto, sizeof(int), 1, f) != 1) break;
        if(fread(&c.cantidad, sizeof(int), 1, f) != 1) break;
        if(fread(&c.comision, sizeof(float), 1, f) != 1){
            // registro cortado (falta la comision): se recalcula como 10% del total
            int pos = buscarProducto(p, nP, c.codigoProducto);
            c.comision = (pos >= 0) ? p[pos].precio * c.cantidad * PORC_COMISION : 0;
            reparado = true;
        }
        dia[n++] = c;
    }
    fclose(f);
    return n;
}

bool guardarDia(const char* archivo, comanda* dia, int n){
    FILE* f = fopen(archivo, "wb");
    if(!f) return false;
    fwrite(dia, sizeof(comanda), n, f);
    fclose(f);
    return true;
}

// ---------------------------------------------------------------- busquedas
int buscarProducto(producto* p, int n, int codigo){
    for(int i = 0; i < n; i++) if(p[i].codigo == codigo) return i;
    return -1;
}
int buscarMozo(mozo* m, int n, int id){
    for(int i = 0; i < n; i++) if(m[i].idMozo == id) return i;
    return -1;
}

// ---------------------------------------------------------------- venta
bool validarMozo(mozo* m, int n, int id){
    int pos = buscarMozo(m, n, id);
    if(pos < 0){ cout << "No existe un mozo con ese id.\n"; return false; }
    char buf[64], cifrada[64];
    for(int i = 1; i <= INTENTOS; i++){
        leerLinea("Password: ", buf, sizeof(buf));
        if(entradaCerrada) return false;
        cifrar(buf, cifrada);
        if(strcmp(cifrada, m[pos].password) == 0){
            cout << "Mozo validado: " << m[pos].nombre << "\n";
            return true;
        }
        cout << "Password incorrecto (" << i << "/" << INTENTOS << ").\n";
    }
    return false;
}

// ---------------------------------------------------------------- orden y pantalla
void ordenarPorMozo(comanda* d, int n){
    // burbuja estable: dentro de un mismo mozo se conserva el orden de carga
    for(int i = 0; i < n - 1; i++)
        for(int j = 0; j < n - 1 - i; j++)
            if(d[j].idMozo > d[j+1].idMozo){
                comanda aux = d[j]; d[j] = d[j+1]; d[j+1] = aux;
            }
}

void mostrarPlanilla(const char* titulo, const char* fecha, comanda* d, int n,
                     producto* p, int nP, mozo* m, int nM){
    cout << "\n================ " << titulo << " ================\n";
    printf("%-11s %-4s %-15s %-6s %-14s %5s %10s %10s\n",
           "Fecha", "Id", "Mozo", "Cod", "Producto", "Cant", "Total", "Comision");
    float totalDia = 0, comisionDia = 0;
    for(int i = 0; i < n; i++){
        int pp = buscarProducto(p, nP, d[i].codigoProducto);
        int pm = buscarMozo(m, nM, d[i].idMozo);
        float total = (pp >= 0) ? p[pp].precio * d[i].cantidad : 0;
        printf("%-11s %-4d %-15s %-6d %-14s %5d %10.2f %10.2f\n",
               fecha, d[i].idMozo, (pm >= 0) ? m[pm].nombre : "?", d[i].codigoProducto,
               (pp >= 0) ? p[pp].descripcion : "?", d[i].cantidad, total, d[i].comision);
        totalDia += total;
        comisionDia += d[i].comision;
    }
    printf("%-11s %-4s %-15s %-6s %-14s %5s %10.2f %10.2f\n", "", "", "TOTAL DIA", "", "", "", totalDia, comisionDia);
}

void mostrarComisiones(comanda* d, int n, mozo* m, int nM){
    cout << "\n---------------- Comision por mozo ----------------\n";
    for(int i = 0; i < nM; i++){
        float suma = 0;
        for(int j = 0; j < n; j++)
            if(d[j].idMozo == m[i].idMozo) suma += d[j].comision;
        m[i].totalComision = suma;
        printf("%-3d %-20s %10.2f\n", m[i].idMozo, m[i].nombre, m[i].totalComision);
    }
}

void mostrarInventario(producto* p, int n){
    cout << "\n---------------- Inventario actualizado ----------------\n";
    printf("%-6s %-20s %10s %8s\n", "Cod", "Descripcion", "Precio", "Stock");
    for(int i = 0; i < n; i++)
        printf("%-6d %-20s %10.2f %8d\n", p[i].codigo, p[i].descripcion, p[i].precio, p[i].stockActual);
}

// ---------------------------------------------------------------- main
int main(){
    mozo mozos[MAX_MOZOS];
    producto prods[MAX_PROD];
    static comanda dia[MAX_DIA];

    int nM = cargarMozos(mozos);
    int nP = cargarInventario(prods);
    if(nM == 0 || nP == 0){
        cout << "No se pudo leer mozos.dat o inventario.dat.\n";
        return 1;
    }

    char fecha[11];
    pedirFecha(fecha);
    if(entradaCerrada) return 1;

    char archivo[40];
    sprintf(archivo, "comandas_%s.dat", fecha);
    bool reparado;
    int nD = cargarDia(archivo, dia, prods, nP, reparado);
    cout << "Archivo " << archivo << ": " << nD << " comandas previas.\n";
    if(reparado)
        cout << "Aviso: el ultimo registro estaba incompleto (sin comision); se recalculo como 10% del total y quedara corregido al guardar.\n";
    if(nD > 0)
        mostrarPlanilla("COMANDAS PREVIAS DEL DIA", fecha, dia, nD, prods, nP, mozos, nM);

    int ventas = 0;
    while(!entradaCerrada){
        cout << "\n--- Nueva venta (codigo 0 para terminar) ---\n";
        int cod;
        if(!leerEntero("Codigo de producto: ", cod)){
            if(!entradaCerrada) cout << "Dato invalido.\n";
            continue;
        }
        if(cod == 0) break;

        int pos = buscarProducto(prods, nP, cod);
        if(pos < 0){ cout << "Producto inexistente.\n"; continue; }
        printf("%s - $%.2f - stock: %d\n", prods[pos].descripcion, prods[pos].precio, prods[pos].stockActual);

        comanda c;
        c.codigoProducto = cod;
        if(!leerEntero("Cantidad: ", c.cantidad) || c.cantidad <= 0){
            if(!entradaCerrada) cout << "Cantidad invalida. Venta cancelada.\n";
            continue;
        }
        if(c.cantidad > prods[pos].stockActual){
            cout << "Stock insuficiente. Venta cancelada.\n";
            continue;
        }

        float total = prods[pos].precio * c.cantidad;
        c.comision = total * PORC_COMISION;
        printf("Total: $%.2f  (comision del mozo 10%%: $%.2f)\n", total, c.comision);

        if(!leerEntero("Id del mozo: ", c.idMozo)){
            if(!entradaCerrada) cout << "Dato invalido. Venta cancelada.\n";
            continue;
        }
        if(!validarMozo(mozos, nM, c.idMozo)){
            if(!entradaCerrada) cout << "Mozo no validado. Venta cancelada.\n";
            continue;
        }
        if(nD >= MAX_DIA){
            cout << "La planilla del dia esta llena.\n";
            break;
        }

        // la venta pasa a la planilla del dia
        dia[nD++] = c;
        prods[pos].stockActual -= c.cantidad;   // solo las ventas de esta sesion descuentan stock
        ventas++;
        cout << "Venta registrada.\n";
    }

    // cierre: inventario, planilla ordenada y pantalla
    ordenarPorMozo(dia, nD);
    if(!guardarInventario(prods, nP)) cout << "Error al guardar inventario.dat\n";
    if(!guardarDia(archivo, dia, nD))  cout << "Error al guardar " << archivo << "\n";

    cout << "\nVentas cargadas en esta sesion: " << ventas << "\n";
    mostrarPlanilla("PLANILLA DEL DIA (ordenada por mozo)", fecha, dia, nD, prods, nP, mozos, nM);
    mostrarComisiones(dia, nD, mozos, nM);
    mostrarInventario(prods, nP);
    return 0;
}
