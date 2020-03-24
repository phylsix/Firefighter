$(document).ready(function() {
  $('#ts').html('Updated at: ' + data.updateTime);

  $('#bydef').css('font-weight', 'bold');

  var $mxx = 0;
  $('#table2mu2e tr').each(function(i, row) {
    var $row = $(row);
    if ($row.has('th').length) return;       // skip header
    if ($row.children().length < 5) return;  // skip sep row
    if ($row.has('td[rowspan]').length) $mxx = $row.find('td[rowspan]').text();
    var $ctaus = $row.children();
    var $ma = $ctaus.eq(-6).text();
    for (var i = -5; i != 0; i++) {
      var $ctau = $ctaus.eq(i).text();
      var $status = getSignalProdStatus('2mu2e', $mxx, $ma, $ctau);
      var $detail = getSignalProdDetail('2mu2e', $mxx, $ma, $ctau);
      $ctaus.eq(i).css('background-color', bkgColorfy($status));

      $ctaus.eq(i).click({detail: $detail}, function(e) {
        $('#info').html(fmtDetail(e.data.detail));
        $('#table2mu2e tr td').css('font-weight', 'normal');
        $('#table4mu tr td').css('font-weight', 'normal');
        $(this).css('font-weight', 'bold');
        e.stopPropagation();
      })
    }
  })

  $mxx = 0;
  $('#table4mu tr').each(function(i, row) {
    var $row = $(row);
    if ($row.has('th').length) return;       // skip header
    if ($row.children().length < 5) return;  // skip sep row
    if ($row.has('td[rowspan]').length) $mxx = $row.find('td[rowspan]').text();
    var $ctaus = $row.children();
    var $ma = $ctaus.eq(-6).text();
    for (var i = -5; i != 0; i++) {
      var $ctau = $ctaus.eq(i).text();
      var $status = getSignalProdStatus('4mu', $mxx, $ma, $ctau);
      var $detail = getSignalProdDetail('4mu', $mxx, $ma, $ctau);
      $ctaus.eq(i).css('background-color', bkgColorfy($status));

      $ctaus.eq(i).click({detail: $detail}, function(e) {
        $('#info').html(fmtDetail(e.data.detail));
        $('#table2mu2e tr td').css('font-weight', 'normal');
        $('#table4mu tr td').css('font-weight', 'normal');
        $(this).css('font-weight', 'bold');
        e.stopPropagation();
      })
    }
  })

  $('#bydef').click(function(e) {
    var $mxx = 0;
    $('#table2mu2e tr').each(function(i, row) {
      var $row = $(row);
      if ($row.has('th').length) return;       // skip header
      if ($row.children().length < 5) return;  // skip sep row
      if ($row.has('td[rowspan]').length) $mxx = $row.find('td[rowspan]').text();
      var $ctaus = $row.children();
      var $ma = $ctaus.eq(-6).text();
      for (var i = -5; i != 0; i++) {
        var $ctau = $ctaus.eq(i).text();
        var $status = getSignalProdStatus('2mu2e', $mxx, $ma, $ctau);
        $ctaus.eq(i).css('background-color', bkgColorfy($status));
      }
    })

    $mxx = 0;
    $('#table4mu tr').each(function(i, row) {
      var $row = $(row);
      if ($row.has('th').length) return;       // skip header
      if ($row.children().length < 5) return;  // skip sep row
      if ($row.has('td[rowspan]').length) $mxx = $row.find('td[rowspan]').text();
      var $ctaus = $row.children();
      var $ma = $ctaus.eq(-6).text();
      for (var i = -5; i != 0; i++) {
        var $ctau = $ctaus.eq(i).text();
        var $status = getSignalProdStatus('4mu', $mxx, $ma, $ctau);
        $ctaus.eq(i).css('background-color', bkgColorfy($status));
      }
    })

    $('#btns .btn').css('font-weight', 'normal');
    $(this).css('font-weight', 'bold');
    e.stopPropagation();
  })




  $('#bygfe').click(function(e) {
    var $mxx = 0;
    $('#table2mu2e tr').each(function(i, row) {
      var $row = $(row);
      if ($row.has('th').length) return;       // skip header
      if ($row.children().length < 5) return;  // skip sep row
      if ($row.has('td[rowspan]').length) $mxx = $row.find('td[rowspan]').text();
      var $ctaus = $row.children();
      var $ma = $ctaus.eq(-6).text();
      for (var i = -5; i != 0; i++) {
        var $ctau = $ctaus.eq(i).text();
        var $eff = getSignalGFE('2mu2e', $mxx, $ma, $ctau);
        $ctaus.eq(i).css('background-color', bkgColorfyByGFE($eff));
      }
    })

    $mxx = 0;
    $('#table4mu tr').each(function(i, row) {
      var $row = $(row);
      if ($row.has('th').length) return;       // skip header
      if ($row.children().length < 5) return;  // skip sep row
      if ($row.has('td[rowspan]').length) $mxx = $row.find('td[rowspan]').text();
      var $ctaus = $row.children();
      var $ma = $ctaus.eq(-6).text();
      for (var i = -5; i != 0; i++) {
        var $ctau = $ctaus.eq(i).text();
        var $eff = getSignalGFE('4mu', $mxx, $ma, $ctau);
        $ctaus.eq(i).css('background-color', bkgColorfyByGFE($eff));
      }
    })

    $('#btns .btn').css('font-weight', 'normal');
    $(this).css('font-weight', 'bold');
    e.stopPropagation();
  })

})


function getUpdateTime() {
  return data.updateTime;
}

function getSignalProdStatus(channel, mxx, ma, ctau) {
  var $store = data.store;
  var $status_code = -1;
  for (var i = 0; i < $store.length; i++) {
    var $entry = $store[i];
    if (JSON.stringify($entry.identifier) ===
        JSON.stringify([channel, mxx, ma, ctau])) {
      if ($entry.jobstatus==='COMPLETED') {
        $status_code = 0;
      } else if ($entry.jobstatus==='SUBMITTED') {
        $status_code = 1;
      } else if ($entry.jobstatus==='TAPERECALL') {
        $status_code = 2;
      } else if ($entry.jobstatus==='FAILED') {
        $status_code = 3;
      }

      break;
    }
  }

  return $status_code;  // not in system
}


function getSignalProdDetail(channel, mxx, ma, ctau) {
  var $store = data.store;
  var $entry = undefined;
  for (var i = 0; i < $store.length; i++) {
    if (JSON.stringify($store[i].identifier) ===
        JSON.stringify([channel, mxx, ma, ctau])) {
        $entry = $store[i];
        return $entry;
    }
  }
  return $entry;
}


function getSignalGFE(channel, mxx, ma, ctau) {
  var $entry = getSignalProdDetail(channel, mxx, ma, ctau);
  if ($entry===undefined || $entry.genfiltereff==='') return undefined;
  return parseFloat($entry.genfiltereff.split('+-')[0])
}


function bkgColorfy(status) {
  switch (status) {
    case 0: return 'green';
    case 1: return 'orange';
    case 2: return 'DarkCyan';
    case 3: return 'Crimson';
    case -1:
      return 'grey';
    default:
      return 'white';
  }
}


function bkgColorfyByGFE(eff) {
  if (eff===undefined) {
    return 'white';
  } else {
    // return 'green';
    return getColorForPercentage(eff);
  }
}


function fmtDetail(detail) {
  if (!detail) return '<b>UNKNOWN</b>';

  var res = '<ul>';
  res += '<li><b>name</b>: ' + detail.name + '</li>';
  res += '<li><b>YamlName</b>: ' + detail.yamlname + '</li>';
  res += '<li><b>genFilterEfficiency</b>: ' + detail.genfiltereff + '</li>';
  res += '<li><b>status</b>: ' + detail.status + '</li>';
  res += '<li><b>jobStatus</b>: ' + detail.jobstatus + '</li>';
  res += '<li><b>submitDir</b>: ' + detail.submitdir + '</li>';
  res += '<li><b>lastCrabTime</b>: ' + detail.lastcrabtime + '</li>';
  res += '<li><b>lastEosTime</b>: ' + detail.lasteostime + '</li>';
  res += '<li><b>ntupleFileList</b>: <ul>' +
      detail.ntuplefiles
          .map((val) => {
            return '<li>' + val + '</li>';
          })
          .join('') +
      '</ul></li>';
  res += '</ul>';
  return res;
}




function getColorForPercentage(pct) {
  // get colors from here: https://gka.github.io/palettes/
  var percentColors = [
    { pct: 0.0, color: { r: 0xff, g: 0xff, b: 0xc2 } },
    { pct: 0.5, color: { r: 0xff, g: 0x96, b: 0x00 } },
    { pct: 1.0, color: { r: 0x00, g: 0x80, b: 0x97 } }
  ];

  for (var i = 1; i < percentColors.length - 1; i++) {
      if (pct < percentColors[i].pct) {
          break;
      }
  }
  var lower = percentColors[i - 1];
  var upper = percentColors[i];
  var range = upper.pct - lower.pct;
  var rangePct = (pct - lower.pct) / range;
  var pctLower = 1 - rangePct;
  var pctUpper = rangePct;
  var color = {
      r: Math.floor(lower.color.r * pctLower + upper.color.r * pctUpper),
      g: Math.floor(lower.color.g * pctLower + upper.color.g * pctUpper),
      b: Math.floor(lower.color.b * pctLower + upper.color.b * pctUpper)
  };
  return 'rgb(' + [color.r, color.g, color.b].join(',') + ')';
  // or output as hex if preferred
}