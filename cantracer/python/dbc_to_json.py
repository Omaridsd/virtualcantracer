# python/dbc_to_json.pyimport sys
import sys
import io
import subprocess

# Forcer la sortie en UTF-8 dans certains environnements
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')

# ✅ Tentative d'import, installation automatique si manquant
try:
    import cantools
except ImportError:
    print("📦 'cantools' n'est pas installé. Installation automatique en cours...")
    try:
        subprocess.check_call([sys.executable, "-m", "pip", "install", "cantools"])
        import cantools
        print("✅ 'cantools' installé avec succès.")
    except Exception as install_error:
        print(f"❌ Échec de l'installation de cantools : {install_error}")
        sys.exit(1)

import json

# ✅ Vérification des arguments
if len(sys.argv) != 3:
    print("Usage: python dbc_to_json.py input.dbc output.json")
    sys.exit(1)

dbc_file = sys.argv[1]
json_file = sys.argv[2]

try:
    db = cantools.database.load_file(dbc_file)
    messages = []

    for msg in db.messages:
        message_dict = {
            "frame_id": msg.frame_id,                       # clé modifiée ici
            "name": msg.name,
            "is_extended_frame": msg.is_extended_frame,     # clé ajoutée ici
            "signals": []
        }
        for sig in msg.signals:
            signal_dict = {
                "name": sig.name,
                "start": sig.start,
                "length": sig.length,
                "is_big_endian": sig.byte_order == 'big_endian',
                "is_signed": sig.is_signed,
                "factor": sig.scale,
                "offset": sig.offset,
                "unit": sig.unit or ""
            }
            message_dict["signals"].append(signal_dict)
        messages.append(message_dict)

    with open(json_file, "w", encoding="utf-8") as f:
        json.dump(messages, f, indent=4, ensure_ascii=False)

    print(f"Fichier JSON sauvegardé : {json_file}")
except Exception as e:
    print("Erreur lors de la conversion :", str(e))
    sys.exit(1)