
i2DX.layout('chuni', function(ui) {

  function style(index) {
    return {
      width: '63px',
      height: '222px',
      bottom: (222 + 54 * (Math.floor((15-index) / 16))) + 'px',
      marginLeft: (index % 16) * 64  + 'px'
    };
  }
  function style2(index) {
    return {
      width: '63px',
      height: '222px',
      bottom: (54 * (Math.floor((15-index) / 16))) + 'px',
      marginLeft: (index % 16) * 64  + 'px'
    };
  }

  ui.button('jubeat_1', style(0));
  ui.button('jubeat_2', style(1));
  ui.button('jubeat_3', style(2));
  ui.button('jubeat_4', style(3));
  ui.button('jubeat_5', style(4));
  ui.button('jubeat_6', style(5));
  ui.button('jubeat_7', style(6));
  ui.button('jubeat_8', style(7));
  ui.button('jubeat_9', style(8));
  ui.button('jubeat_10', style(9));
  ui.button('jubeat_11', style(10));
  ui.button('jubeat_12', style(11));
  ui.button('jubeat_13', style(12));
  ui.button('jubeat_14', style(13));
  ui.button('jubeat_15', style(14));
  ui.button('jubeat_16', style(15));
  ui.button('jubeat_17', style2(0));
  ui.button('jubeat_18', style2(1));
  ui.button('jubeat_19', style2(2));
  ui.button('jubeat_20', style2(3));
  ui.button('jubeat_21', style2(4));
  ui.button('jubeat_22', style2(5));
  ui.button('jubeat_23', style2(6));
  ui.button('jubeat_24', style2(7));
  ui.button('jubeat_25', style2(8));
  ui.button('jubeat_26', style2(9));
  ui.button('jubeat_27', style2(10));
  ui.button('jubeat_28', style2(11));
  ui.button('jubeat_29', style2(12));
  ui.button('jubeat_30', style2(13));
  ui.button('jubeat_31', style2(14));
  ui.button('jubeat_32', style2(15));
});
