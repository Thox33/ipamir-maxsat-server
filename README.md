# IPAMIR MaxSAT Server

Ein TCP-Server, der die [IPAMIR-Schnittstelle](https://bitbucket.org/coreo-group/ipamir/)
für inkrementelles MaxSAT-Lösen über ein Textprotokoll verfügbar macht. Damit können
Programme, die nicht in C oder C++ geschrieben sind oder in einem eigenen Prozess laufen,
einen inkrementellen MaxSAT-Löser nutzen, ohne ihn einzubinden.

Entstanden als Werkzeug für meine Masterarbeit an der Technischen Universität Hamburg über
inkrementelles MaxSAT-Lösen für die Mehrzieloptimierung in Echtzeit-Mitfahrdiensten.

## Warum

IPAMIR ist eine C-Schnittstelle. Wer einen Löser aus einer anderen Sprache oder aus einem
getrennten Prozess ansprechen will, muss ihn einbetten und das Speichermanagement selbst
verantworten. Ein Löser, der während einer langen Optimierung abstürzt, reißt dann die
aufrufende Anwendung mit.

Dieser Server kapselt den Löser hinter einer Socket-Verbindung. Der Zustand bleibt über
mehrere Anfragen hinweg erhalten, was der eigentliche Zweck einer inkrementellen
Schnittstelle ist: Klauseln hinzufügen, lösen, weitere Klauseln hinzufügen, erneut lösen,
ohne von vorn zu beginnen.

Der Server ist nicht an einen bestimmten Löser gebunden. Er spricht IPAMIR, und jede
IPAMIR-konforme Implementierung lässt sich dagegen linken.

## Protokoll

Zeilenbasiert über TCP, standardmäßig Port 12345.

| Befehl | Bedeutung |
|---|---|
| `ADD_HARD_CLAUSE <literale>` | Harte Klausel hinzufügen |
| `ADD_SOFT_CLAUSE <literale> <gewicht>` | Weiche Klausel mit Gewicht hinzufügen |
| `ASSUME <literal>` | Annahme für den nächsten Lösungsaufruf |
| `SOLVE` | Lösen, liefert Ergebnis und Zielfunktionswert |
| `QUERY_LIT <literal>` | Belegung eines Literals im gefundenen Modell abfragen |
| `INTERRUPT` | Laufenden Lösungsvorgang abbrechen |
| `RESTART_SOLVER` | Löser-Instanz verwerfen und neu anlegen |
| `EXIT` | Verbindung beenden |

Unter `src/example_client/` liegt ein kleiner C++-Client, der das Protokoll vorführt.

## Aufbau

```text
src/server/           Server, Socket-Behandlung, Protokoll
  server.cc           Verbindungsannahme, Puffer- und Nachrichtenverarbeitung
  ipamir_solver.cc    dünne Hülle um die IPAMIR-Aufrufe
src/example_client/   Beispielclient
test/                 Test gegen die Löser-Hülle
```

Rund 650 Zeilen C++.

## Bauen

**Voraussetzungen, und hier liegt die Hürde:**

1. Ein gebauter, IPAMIR-konformer MaxSAT-Löser. Entwickelt wurde gegen
   [incremental-maxhs](https://bitbucket.org/coreo-group/incremental-maxhs) (iMaxHS, MIT-Lizenz).
2. **IBM CPLEX Optimizer.** iMaxHS nutzt CPLEX als MIP-Löser. CPLEX ist kommerziell;
   für den akademischen Gebrauch gibt es kostenlose Lizenzen.
3. Linux, g++ mit C++11.

```bash
cd src/server
make IPAMIR_DIR=/pfad/zum/gebauten/solver CPLEX_LIB_DIR=/pfad/zu/cplex/lib
./server
```

Die Pfade lassen sich auch als Umgebungsvariablen setzen. Tests entsprechend:

```bash
cd test
make test IPAMIR_DIR=/pfad/zum/gebauten/solver CPLEX_LIB_DIR=/pfad/zu/cplex/lib
```

## Stand

Dies ist der Stand aus der Masterarbeit, aufgeräumt und mit konfigurierbaren Pfaden
versehen. Er hat seinen Zweck dort erfüllt, ist aber kein gepflegtes Produkt: keine
Authentifizierung, ein Client zur Zeit, Fehlerbehandlung auf das Nötige beschränkt.

Veröffentlicht, weil die Kapselung einer Standardschnittstelle hinter einem Prozess eine
Lösung ist, die ich anderswo nicht gefunden habe, und weil der Code ein ehrliches Bild
meiner Arbeit zu dieser Zeit gibt.

**Ich kann nicht nachweisen, dass er bei dir baut.** Mir steht derzeit keine
CPLEX-Lizenz zur Verfügung. Wer es versucht und Probleme findet, darf sie gern melden.

## Lizenz

MIT, siehe [LICENSE](LICENSE).

Der Löser selbst gehört nicht zu diesem Repository. iMaxHS stammt von Jessica Davies,
Fahiem Bacchus, Jeremias Berg und Andreas Niskanen und steht ebenfalls unter MIT-Lizenz.
