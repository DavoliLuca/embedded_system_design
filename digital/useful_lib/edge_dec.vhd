use IEEE.STD_LOGIC_1164.ALL;

entity edge_det is
    Port ( X    : in  std_logic;
           Y    : out std_logic;
           ck   : in  std_logic);
end edge_det;

architecture Behavioral of edge_det is
signal oldValue         : std_logic;
signal currentValue     : std_logic;
begin


 process (ck) is
 begin
        if (ck='1' and ck'event) then
                -- salvataggio vecchio valore
                oldValue <= currentValue;
                -- leggo il valore corrente
                currentValue <= X;
        end if;
 end process;

  -- generazione dell'uscita in base alla transizione richiesta
  -- in questo caso cerco fronti di salita

  Y <= currentValue and (not oldValue);  -- y = 1 se currrent=1 e old=0

end Behavioral;