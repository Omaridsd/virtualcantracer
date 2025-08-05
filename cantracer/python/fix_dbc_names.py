import re
import sys

def clean_dbc(input_path, output_path):
    prefix_for_numbers = '_'

    # Regex patterns
    msg_line_pattern = re.compile(r'^(BO_)\s+(\d+)\s+(\S+):\s*(\d+)\s+(\S+)$')
    signal_line_pattern = re.compile(r'^(SG_)\s+(\S+):\s*(.+)$')
    val_line_pattern = re.compile(r'^(VAL_)\s+(\d+)\s+(.+)$')
    fieldtype_pattern = re.compile(r'.*FieldType.*')

    def fix_name(name):
        # Ajoute un underscore si le nom commence par un chiffre
        if re.match(r'^\d', name):
            return prefix_for_numbers + name
        return name

    def fix_signal_line(line):
        # Corrige les espaces dans la partie "start|length@..." de la ligne SG_
        # Ex: "1 | 1@1 +" => "1|1@1 +"
        # Trouve la partie start|length@1+ ou similaire
        # Pattern dans SG_: nom: start|length@1+ (facteur, offset)[min|max] "unit" Receiver
        parts = line.split(':', 1)
        if len(parts) != 2:
            return line
        prefix, rest = parts
        # Trouver la première occurrence du pattern start|length@order+ ou -
        m = re.search(r'(\d+)\s*\|\s*(\d+)@(\d)([+-])', rest)
        if m:
            corrected = f"{m.group(1)}|{m.group(2)}@{m.group(3)}{m.group(4)}"
            # Remplacer la première occurrence
            rest = rest[:m.start()] + corrected + rest[m.end():]
        return f"{prefix}:{rest}"

    with open(input_path, 'r', encoding='utf-8') as f_in, open(output_path, 'w', encoding='utf-8') as f_out:
        for line in f_in:
            # Supprimer les lignes avec "FieldType"
            if fieldtype_pattern.match(line):
                # ignore line
                continue

            # Nettoyer les lignes messages BO_
            m_msg = msg_line_pattern.match(line)
            if m_msg:
                prefix, msg_id, msg_name, dlc, node = m_msg.groups()
                new_name = fix_name(msg_name)
                # Vérifier la validité de l'ID standard 11 bits max
                id_int = int(msg_id)
                if id_int > 0x7FF:
                    print(f"Warning: Message ID {msg_id} dépasse 11 bits (ligne ignorée)")
                    continue  # Ignore ou adapte ici selon besoin
                f_out.write(f"{prefix} {msg_id} {new_name}: {dlc} {node}\n")
                continue

            # Nettoyer les lignes signaux SG_
            m_sig = signal_line_pattern.match(line)
            if m_sig:
                prefix, sig_name, rest = m_sig.groups()
                new_sig_name = fix_name(sig_name)
                fixed_line = fix_signal_line(line)
                # Remplacer le nom dans la ligne fixe
                # la ligne fix_signal_line garde le nom ancien, donc on remplace
                fixed_line = re.sub(r'^(SG_)\s+\S+:', f'\\1 {new_sig_name}:', fixed_line)
                f_out.write(fixed_line + '\n')
                continue

            # Nettoyer les lignes VAL_ (exemple simple : corriger les noms avec chiffres)
            m_val = val_line_pattern.match(line)
            if m_val:
                prefix, msg_id, rest = m_val.groups()
                # Ne fait rien de spécial ici mais peut être étendu
                f_out.write(line)
                continue

            # Sinon écrire la ligne telle quelle
            f_out.write(line)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python clean_dbc.py input.dbc output_fixed.dbc")
        sys.exit(1)
    input_dbc = sys.argv[1]
    output_dbc = sys.argv[2]
    clean_dbc(input_dbc, output_dbc)
    print(f"Fichier nettoyé enregistré dans : {output_dbc}")
